.POSIX:
.SUFFIXES:

VERSION != git describe --tags 2>/dev/null || echo 0.0

CC = clang
CPPFLAGS = -D_DEFAULT_SOURCE -DVERSION=\"$(VERSION)\"
CFLAGS   = -std=c2x -g3 -Wpedantic -Wall -Wextra -O3              \
	-fdiagnostics-color=always -fsanitize=undefined,address,leak  \
	-fstack-protector-all -fno-omit-frame-pointer -fno-common     \
	-Wvla -Wshadow -Wdouble-promotion -Wnull-dereference          \
	-Wconversion -Wfloat-equal -Wformat=2 -Wstrict-aliasing=2     \
	-Wcast-qual -Wdate-time -Wimplicit-fallthrough -Wpacked       \
	-Wmissing-include-dirs -Wnested-externs -Wstack-protector     \
	-Wunused-parameter -Wredundant-decls -Wundef -Winit-self      \
	-Wold-style-definition -Wstrict-prototypes
#-s
LDFLAGS  = -fuse-ld=lld                   \
	-Xlinker --error-unresolved-symbols   \
	-Xlinker --color-diagnostics=always   \
	-Xlinker --gc-sections
LDLIBS   =

all: keygen server/server client/client
LIBHYDRO_COMPS = libhydrogen/hydrogen
SERVER_COMPS = server/server
CLIENT_COMPS = client/client

crypto.o: crypto.c crypto.h $(LIBHYDRO_COMPS:=.h) makefile
keyfile.o: crypto.c crypto.h $(LIBHYDRO_COMPS:=.h) makefile
keygen.o: keygen.c keyfile.h crypto.h $(LIBHYDRO_COMPS:=.h) makefile
telemutil.o: telemutil.c telemutil.h telemetry.h makefile
server.o: $(SERVER_COMPS:=.c) keyfile.h crypto.h $(LIBHYDRO_COMPS:=.h) makefile
client.o: $(CLIENT_COMPS:=.c) keyfile.h crypto.h $(LIBHYDRO_COMPS:=.h) makefile
libhydrogen.o: $(LIBHYDRO_COMPS:=.c) $(LIBHYDRO_COMPS:=.h) makefile

keygen: keygen.o keyfile.o keyfile.h crypto.o crypto.h $(LIBHYDRO_COMPS:=.o) $(LIBHYDRO_COMPS:=.h) makefile
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) keygen.o keyfile.o crypto.o $(LIBHYDRO_COMPS:=.o) $(LDLIBS)

server/server: $(SERVER_COMPS:=.o) keyfile.o keyfile.h crypto.o crypto.h telemutil.o telemutil.h $(LIBHYDRO_COMPS:=.o) $(LIBHYDRO_COMPS:=.h) makefile
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(SERVER_COMPS:=.o) keyfile.o crypto.o telemutil.o $(LIBHYDRO_COMPS:=.o) $(LDLIBS)

client/client: $(CLIENT_COMPS:=.o) keyfile.o keyfile.h crypto.o crypto.h telemutil.o telemutil.h $(LIBHYDRO_COMPS:=.o) $(LIBHYDRO_COMPS:=.h) makefile
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(CLIENT_COMPS:=.o) keyfile.o crypto.o telemutil.o $(LIBHYDRO_COMPS:=.o) $(LDLIBS)

clean:
	rm -f server/server client/client keygen *.o *.priv *.pub

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
