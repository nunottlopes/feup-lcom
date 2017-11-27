#include "i8042.h"

.global _kbdass

.text
_kbdass:

	inb $STAT_REG, %al
	testb $OUT_BUF_FULL, %al
	jz _kbdass
	inb $OUT_BUF, %al

	ret
