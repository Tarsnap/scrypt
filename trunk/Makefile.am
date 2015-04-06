AUTOMAKE_OPTIONS=	foreign

bin_PROGRAMS=	scrypt
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c				\
			lib/util/memlimit.c		\
			libcperciva/util/readpass.c		\
			libcperciva/util/warnp.c		\
			lib/scryptenc/scryptenc.c	\
			lib/scryptenc/scryptenc_cpuperf.c	\
			libcperciva/crypto/crypto_aesctr.c	\
			libcperciva/alg/sha256.c		\
			lib/crypto/crypto_scrypt-@SCRYPTVER@.c	\
			libcperciva/cpusupport/cpusupport_x86_aesni.c \
			cpusupport-config.h
scrypt_CFLAGS=		-I $(srcdir)/lib/util -I $(srcdir)/lib/scryptenc \
			-I $(srcdir)/lib/crypto -I $(srcdir)/libcperciva/alg \
			-I $(srcdir)/libcperciva/crypto	-I $(srcdir)/libcperciva/util \
			-I $(srcdir)/libcperciva/cpusupport \
			-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"
scrypt_LDADD=		-lcrypto
scrypt_man_MANS=	scrypt.1

cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; cd libcperciva/cpusupport/Build && command -p sh cpusupport.sh ) > cpusupport-config.h
BUILT_SOURCES=	cpusupport-config.h
