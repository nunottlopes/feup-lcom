#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
  
	mmap_t newmap;

	if(lm_init() == NULL)
		return 1;

	if(lm_alloc(sizeof(vbe_mode_info_t), &newmap) == NULL)
		return 1;

	struct reg86u r;

	r.u.w.ax = 0x4F01;
	r.u.w.es = PB2BASE(newmap.phys);
	r.u.w.di = PB2OFF(newmap.phys);
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if( sys_int86(&r) != OK ) {
		printf("sys_int86() failed\n");
		return 1;
	}

	*vmi_p = *((vbe_mode_info_t *)newmap.virtual);

	lm_free(&newmap);
	return 0;
}


