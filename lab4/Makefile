# Makefile for lab4

COMPILER_TYPE= gnu

CC= gcc

clean:
	rm *.d
	rm .depend

PROG= lab4
SRCS= lab4.c test4.c mouse.c timer.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
