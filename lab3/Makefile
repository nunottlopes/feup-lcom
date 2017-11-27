# Makefile for lab3

COMPILER_TYPE= gnu

CC= gcc

clean:
	rm *.d
	rm .depend

PROG= lab3
SRCS= lab3.c test3.c kbd.c kbdass.S timer2.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
