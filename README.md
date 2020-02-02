## mqproxy

mqproxy is a simple [ZeroMQ proxy](http://api.zeromq.org/3-2:zmq-proxy)
and forwarder.

Note that you need [the newest version of libzmq](https://github.com/zeromq/libzmq)
in order to build.

### Build

You need a C++ compiler like GCC, CMake and the headers for libzmq
(like libzmq3-dev).

```
mkdir build
cd build
cmake ..
make
```
