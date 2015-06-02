ifeq ($(PREFIX),)
PREFIX = /usr
endif

CFLAGS=-g -fpic 
ifeq ($(LOG_LEVEL),)
  CFLAGS += -D LOG_LEVEL=3
  $(info setting log level to 3)
else
  CFLAGS += -D LOG_LEVEL=$(LOG_LEVEL)
  $(info setting log level to $(LOG_LEVEL))
endif

SRCS = $(wildcard *.c)
ifeq ($(SRCS),)                  #work around wildcard function bug in GNU Make 3.77
SRCS = $(shell echo *.c)
endif

OBJS = $(SRCS:.c=.o)

all: libsnp.so

libsnp.so: $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) -lsctp

install:
	mkdir -p $(PREFIX)/include/libsnp $(PREFIX)/lib
	cp *.h   $(PREFIX)/include/libsnp
	cp *.so  $(PREFIX)/lib

uninstall: clean
	rm -rf $(PREFIX)/include/libsnp $(PREFIX)/lib/libsnp.so

clean:
	rm *.o *.so
