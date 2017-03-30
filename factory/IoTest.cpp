#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "PinTest.h"
#include "I2CTest.h"
#include "SPITest.h"
#include "ADCTest.h"
#include "IoTest.h"


int IOTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
			{ "I/O Function Test" /*title*/,          		0,NULL },
			{ "I2C",										0,I2CTest_Menu },
			{ "SPI0",										0,SPITest_Menu },
//          	{ "SPI1"		,									1,SPITest_Menu },
//			{ "GPIO",										0,NULL },
			{ "PWM",										0,NULL },
			{ "ADC",										0,ADCTest_Menu },
//			{ "Watchdog",									0,NULL },
			{ "GPIO Test",									0,PinTest_Menu },
			{ NULL,											0,NULL }
	};

	return simple_cli_list(cmds);
}