#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "lsm303ah/apollon_lsm303ah_spi_generated.h"
#include "ism330dlc/apollon_ism330dlc_spi_generated.h"

int main(int argc, char *argv[]) {

	printf("Start test for sensors lsm303ah and ism330dlc\n");	
	uint8_t res = 0, value_xl = 0, value_mg = 0;

	// write_lsm303ah(0x21,0x05);
	apollon_lsm303ah_spi_set_option(0x21, 0x05);
	// value_xl = read_lsm303ah(0x0f);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_lsm303ah_spi_get_option(0x0f);
	printf("v= %d. try next data!\n", value_xl);
	if (value_xl == 0x43)
	{
		printf("whoami xl test success!");
		// value_mg = read_lsm303ah(0x4f);
		value_mg = apollon_lsm303ah_spi_get_option(0x4f);
 		if(0x40 == value_mg)      res = 1;
		else if(0x41 == value_mg)     res = 1;
		else
		{
			printf("howami mg test failed!");
		}
		
	}
	else
	{
		printf("whoami xl test failed!");
	}

	res = 0;

	apollon_ism330dlc_spi_set_option(0x12, 0x0C);
	value_xl = apollon_ism330dlc_spi_get_option(0x0F);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_ism330dlc_spi_get_option(0x0F);
	printf("v= %d. try next data!\n", value_xl);
	value_xl = apollon_ism330dlc_spi_get_option(0x0F);
	printf("v= %d. try next data!\n", value_xl);
	if (value_xl == 0x6A)
	{
		printf("whoami test for ism330dlc done!");
	}
	if (res)
	{
		printf("All done right!\n");
		res = 0;
	}
	else
	{
		printf("something gone wrong!\n");
	}
	

	return res;
}