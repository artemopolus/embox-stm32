#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "lsm303ah/apollon_lsm303ah_spi_generated.h"

int main(int argc, char *argv[]) {

	printf("Start test for sensors lsm303ah and ism330dlc\n");	
	uint8_t res = 0, value_xl = 0, value_mg = 0;

	// write_lsm303ah(0x21,0x05);
	apollon_lsm303ah_spi_set_option(0x21, 0x05);
	// value_xl = read_lsm303ah(0x0f);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	if (value_xl == 0x43)
	{
		// value_mg = read_lsm303ah(0x4f);
		value_mg = apollon_lsm303ah_spi_get_option(0x4f);
 		if(0x40 == value_mg)      res = 1;
		if(0x41 == value_mg)     res = 1;
	}


	return 0;
}