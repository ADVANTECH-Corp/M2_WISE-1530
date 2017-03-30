#include "mbed.h"
#include "rtos.h"
#include "rtc_api.h"



Serial uart1(SERIAL_TX, SERIAL_RX, 115200);
Serial uart2(UART2_TX, UART2_RX, 115200);

DigitalOut CB_PWR_ON(GPIO2);
DigitalOut nCB_RESET_OUT(GPIO7);
DigitalOut LED0(GPIO0);
DigitalOut LED1(GPIO1);

static void  Standby_Mode(void){
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

	/* Clear the related wakeup pin flag */
  	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);
	uart1.printf("Enter standby\r\n");
	wait_ms(10);
	HAL_PWR_EnterSTANDBYMode();
}

void LED_thrd(void)
{
	while(1){
		LED0=!LED0;
		LED1=!LED1;
		wait_ms(250);
	}
}

void Ptr_thrd(void)
{
	//static int i=0;
	while(1){
		time_t t=time(NULL);
		uart1.printf("%s.\r\n", ctime(&t));
		wait_ms(1000);
	}
}


// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
	Thread tLED(LED_thrd);
	Thread tPrt(Ptr_thrd);


	CB_PWR_ON=1;
	nCB_RESET_OUT=1;
	rtc_init();
	//HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);
	
	uart1.printf("Wise-1530 Standby_Mode test by Uart1.\r\n");
	wait_ms(100);
	uart2.printf("wise-1530 Standby_Mode test by Uart2.\r\n");
	LED0=1;
	LED1=0;
	set_time(1256729737);
	while(1) {
		;
    }    
}



