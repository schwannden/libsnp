# libsnp, schwannden's network programming library

### To compile
```make```

### To install
```sudo make install```

The default ```prefix``` is ```/usr```

The headers are installed in $(PREFIX)/includes/libsnp

The library is installed in $(PREFIX)/lib

Customize your ```PREFIX``` by

```sudo make install PREFIX=/yourpath```

### To uninstall
```sudo make uninstall```

Default uninstall ```PREFIX``` is ```/usr```

If you installed the library in different PREFIX, remember to add correct prefix by

```sudo make uninstall PREFIX=/yourpath```
