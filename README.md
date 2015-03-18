[![Build Status](https://travis-ci.org/iptube/SPUDlib.svg?branch=master)](https://travis-ci.org/iptube/SPUDlib)
[![Coverage Status](https://coveralls.io/repos/iptube/SPUDlib/badge.svg?branch=master)](https://coveralls.io/r/iptube/SPUDlib?branch=master)
# SPUDlib
Substrate Protocol for User Datagrams (SPUD) Prototype

## Compiling

    ./bootstrap.sh   (To create the ./configure script)
    ./configure   (To create the Makefiles)
    make          (To build the code)

You will end up with two test binaries in samplecode and a library you can link against in src.

sampelcode/spudecho is a simple echo-server that echoes back all UDP packets.
samplecode/spudtest [iface] [ipaddr] sends SPUD packets to the desired IP from the given interface

## Development

### Unit Tests
Turn on unit tests with:

    ./configure --enable-check

This need the check library installed

Build and run the checks with

    make check

If tests fail it can help to run
test/check_spudlib to see where it fails.

(On ubuntu I did not get the ubuntu packaged version of check to work, but
downloading and installing manually did work like a charm. But remember to
uninstall the ubuntu check package first.)

Travis will compile and run the tests.
(https://travis-ci.org/iptube/SPUDlib/)

### Coverage

Coveralls use the coverage.sh script to generate coverage reports.
(https://coveralls.io/r/iptube/SPUDlib)

To manually generate lcov reports you can run the ./coveragereport.sh script.
A nice html page can be found in lcov/index.html

It can be a bit tricky to get your favorite platform to support gcov and
friends, but do not give up! (I had problems on os-x, but unfortunately I do not
remember how it was fixed)

### Coding Standard

TODO: make this more formal.

We like to do C90/C99ish style. Will make sure the library compile on most platforms.

Braces can be placed where you want them. But try to be consistent..

White-spaces when committing should be avoided. (Usual rant on diffs and
readability)

## Contributing

Fork and send pull requests.  Please add your name to the AUTHORS list in your
first patch.

# License

BSD 2-clause:

Copyright (c) 2015, IPTube AUTHORS
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
