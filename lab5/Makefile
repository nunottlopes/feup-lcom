# Makefile for lab5

COMPILER_TYPE= gnu

CC= gcc

clean:
	rm *.d
	rm .depend

PROG= lab5
SRCS= lab5.c test5.c vbe.c video_gr.c video_test.c kbd.c read_xpm.c sprite.c timer.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
