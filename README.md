# libsnp: schwannden's network programming library

### Pre-requisites
We need ``lksctp-tools``
```
git clone git://github.com/borkmann/lksctp-tools.git
cd lksctp-tools
git checkout lksctp-tools-1.0.16
./bootstrap
./configure
make
make install
```
By default this installs the sctp libraries in ``/usr/local/lib/libsctp.so``

### To compile

``make``

There are currently 6 logging levels, that can be set during compilation

``LOG_LEVEL=n make``

where n could be
* 0: no logging
* 1: only error messgaes
* 2: warning messages and above
* 3: infomative messages and above
* 4: debug messages and above
* 5: all messages, same as above

For example, the default is

``LOG_LEVEL=3 make``

Which will generate all informative, error, and warning messages.

### To install

``sudo make install``

The default ``prefix`` is ``/usr``

The headers are installed in ``$(PREFIX)/includes/libsnp``

The library is installed in ``$(PREFIX)/lib``

Customize your ``PREFIX`` by

``sudo make install PREFIX=/yourpath``

### To uninstall

``sudo make uninstall``

Default uninstall ``PREFIX`` is ``/usr``

If you installed the library in different ``PREFIX``, remember to add correct prefix by

``sudo make uninstall PREFIX=/yourpath``

