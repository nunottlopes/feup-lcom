#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "game.h"


int main(int argc, char **argv){

	sef_startup();

	open_game();

	return 0;
}
