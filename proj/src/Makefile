# Makefile for the proj

COMPILER_TYPE= gnu

CC= gcc

clean:
	rm *.d
	rm .depend

PROG= proj
SRCS= proj.c kbd.c read_xpm.c sprite.c timer.c vbe.c video_gr.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>