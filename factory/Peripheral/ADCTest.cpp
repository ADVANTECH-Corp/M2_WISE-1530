#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "ADCTest.h"

#define console_printf ((Serial *)printer)->printf

static AnalogIn adc0(ADCONV0);
static AnalogIn adc1(ADCONV1);
static AnalogIn adc2(ADCONV2);
static AnalogIn adc3(ADCONV3);

static int Read(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    AnalogIn *adc = NULL;
    switch(index) {
        case 0:
            adc = &adc0;
        break;
        
        case 1:
            adc = &adc1;
        break;
        
        case 2:
            adc = &adc2;
        break;
        
        case 3:
            adc = &adc3;
        break;
    }
    if(adc == NULL) return 0;
    
	console_printf("\r\n");
	console_printf("Result: %f\n",adc->read() * 3.3);
	console_printf("\r\n");
	return true;
}

int ADCTestByIndex(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "ADC-IN Test" /*title*/,  				NULL,NULL },
            { "Read",							        index,Read },
            { NULL,										NULL,NULL }
    };
	return simple_cli_list(cmds);
}

int ADCTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "ADC list" /*title*/,                 NULL,NULL },
            { "ADC_0",                              0,ADCTestByIndex },
            { "ADC_1",                              1,ADCTestByIndex },
            { "ADC_2",                              2,ADCTestByIndex },
            { "ADC_3",                              3,ADCTestByIndex },
            { NULL,                                 NULL,NULL }
    };
	return simple_cli_list(cmds);
}

