//#include <bitstring.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <signal.h>

#include "spudlib.h"
#include "tube.h"
#include "iphelper.h"
#include "sockethelper.h"
#include "htable.h"

#define MYPORT 1402    // the port users will be connecting to
#define MAXBUFLEN 2048
#define MAX_LISTEN_SOCKETS 10

int sockfd = -1;
ls_htable clients = NULL;

typedef struct _context_t {
    size_t count;
} context_t;

context_t *new_context() {
    context_t *c = malloc(sizeof(context_t));
    c->count = 0;
    return c;
}

void teardown()
{
    printf("Quitting...\n");
    if (sockfd >= 0) {
        close(sockfd);
    }
    exit(0);
}

static void read_cb(tube_t *tube,
                    const uint8_t *data,
                    ssize_t length,
                    const struct sockaddr* addr)
{
    UNUSED_PARAM(addr);

    // echo
    tube_data(tube, (uint8_t*)data, length);
    ((context_t*)tube->data)->count++;
}

static void close_cb(tube_t *tube,
                     const struct sockaddr* addr)
{
    UNUSED_PARAM(addr);
    context_t *c = (context_t*)tube->data;
    char idStr[SPUD_ID_STRING_SIZE+1];

    printf("Spud ID: %s CLOSED: %zd data packets\n",
           spud_idToString(idStr,
                           sizeof(idStr),
                           &tube->id),
           c->count);
    tube_t *old = ls_htable_remove(clients, &tube->id);
    if (old != tube) {
        fprintf(stderr, "Invalid state closing tube\n");
    }
    free(c);
    free(tube);
}

static int socketListen() {
    struct sockaddr_storage their_addr;
    uint8_t buf[MAXBUFLEN];
    char idStr[SPUD_ID_STRING_SIZE+1];
    socklen_t addr_len;
    int numbytes;
    tube_t *tube;
    spud_message_t sMsg;
    ls_err err;
    spud_flags_id_t uid;

    addr_len = sizeof their_addr;

    while(1) {
        addr_len = sizeof(their_addr);
        if ((numbytes = recvfrom(sockfd, buf,
                                 MAXBUFLEN , 0,
                                 (struct sockaddr *)&their_addr,
                                 &addr_len)) == -1) {
            perror("recvfrom (data)");
            return 1;
        }

        if (!spud_cast(buf, numbytes, &sMsg)) {
            // it's an attack.  Move along.
            continue;
        }

        spud_copyId(&sMsg.header->flags_id, &uid);

        tube = (tube_t *)ls_htable_get(clients, &uid);
        if (!tube) {
            // get started
            tube = malloc(sizeof(tube_t));
            if (!tube) {
                fprintf(stderr, "out of memory");
                return 1; // TODO: replace with an UNUSED_PARAM queue
            }
            tube_init(tube, sockfd);
            tube->data = new_context();
            tube->data_cb = read_cb;
            tube->close_cb = close_cb;
            if (!ls_htable_put(clients, &uid, tube, NULL, &err)) {
                fprintf(stderr, "ls_htable_put: %d, %s", err.code, ls_err_message(err.code));
            }

            printf("Spud ID: %s created\n",
                   spud_idToString(idStr, sizeof(idStr), &uid));

        }
        tube_recv(tube, &sMsg, (struct sockaddr *)&their_addr);
    }
    return 0;
}

unsigned int hash_id(const void *id) {
    // treat the 8 bytes of tube ID like a long long.
    uint64_t key = *(uint64_t *)id;

    // from
    // https://gist.github.com/badboy/6267743#64-bit-to-32-bit-hash-functions
    key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (unsigned int) key;
}

int compare_id(const void *key1, const void *key2) {
    int ret = 0;
    uint64_t k1 = *(uint64_t *)key1;
    uint64_t k2 = *(uint64_t *)key2;
    if (k1<k2) {
        ret = -1;
    } else {
        ret = (k1==k2) ? 0 : 1;
    }
    return ret;
}

int main(void)
{
    struct sockaddr_in6 servaddr;
    ls_err err;
    signal(SIGINT, teardown);

    // 65521 is max prime under 65535, which seems like an interesting
    // starting point for scale.
    if (!ls_htable_create(65521, hash_id, compare_id, &clients, &err)) {
        fprintf(stderr, "ls_htable_create: %d, %s\n", err.code, ls_err_message(err.code));
        return 1;
    }

    sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }
    sockaddr_initAsIPv6Any(&servaddr, MYPORT);
    if (bind(sockfd, (struct sockaddr*)&servaddr, tube_getSockAddrLen( (struct sockaddr*)&servaddr )) != 0) {
        perror("bind");
        return 1;
    }

    return socketListen();
}
