#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "lsm303ah/apollon_lsm303ah_spi_generated.h"
#include "ism330dlc/apollon_ism330dlc_spi_generated.h"
#include "bmp280/apollon_bmp280_i2c_generated.h"
#include "tim/apollon_tim_generated.h"
#include "spi/spi2_fullduplex_slave.h"

int main(int argc, char *argv[]) {

	printf("enable tim");
	apollon_tim_enable();
	printf("Start test for sensors lsm303ah\n");	
	uint8_t res = 0, value_xl = 0;
	uint8_t value_mg = 0;

	//write_lsm303ah(0x21,0x05);
	apollon_lsm303ah_spi_set_option(0x21, 0x05);
	// value_xl = read_lsm303ah(0x0f);
	uint8_t address_lsm303ah = 0x0f;
	for (uint8_t i = 0; i < 10; i++)
	{
		/* code */
		value_xl = apollon_lsm303ah_spi_get_option(address_lsm303ah);
		printf("Get value from %#04x = %d ( %#04x )\n", address_lsm303ah, value_xl, value_xl);
	}
	
	if (value_xl == 0x43)
	{
		printf("whoami xl test success!\n");
		// value_mg = read_lsm303ah(0x4f);
		value_mg = apollon_lsm303ah_spi_get_option(0x4f);
 		if(0x40 == value_mg)      res = 1;
		else if(0x41 == value_mg)     res = 1;
		if(res)
			printf("whoami success!\n");
		else
		{
			printf("howami mg test failed!\n");
		}
		
	}
	else
	{
		printf("whoami xl test failed!\n");
	}
	printf("Start test for sensors ism330dlc\n");	

	res = 0;

	apollon_ism330dlc_spi_set_option(0x12, 0x0C);
	
	uint8_t address_ism330dlc = 0x0f;	
	for (uint8_t i = 0; i < 10; i++)
	{
		value_xl = apollon_ism330dlc_spi_get_option(address_ism330dlc);
		printf("Get value from %#04x = %d ( %#04x )\n", address_ism330dlc, value_xl, value_xl);

	}
	
	
	if (value_xl == 0x6A)
	{
		printf("whoami test for ism330dlc done!\n");
		res = 1;
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

	printf("Start test for bmp280\n");

	const uint8_t bmp280_whoami_reg = 0xd0;
	const uint8_t bmp280_whoami_val = 0x58;

	value_xl = apollon_bmp280_spi_get_option(bmp280_whoami_reg);

	printf("Get WhoAmI [%#04x]=[%#04x]:", bmp280_whoami_reg, value_xl);

	if (value_xl == bmp280_whoami_val)
		printf("OK");
	else
		printf("Failed!");
	
	printf("\n");


	return res;
}