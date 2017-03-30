#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "SPITest.h"

#define console_printf ((Serial *)printer)->printf



//HDC1050 Slave
#define N25Q032A_CMD_READ_ID        0x9F// 0x9E/0x9F

//HDC1050 Offset
#define N25Q032A_MANUFACTURER_ID    0x20//	Byte 1 Manufacturer ID

#define SPI_MAX_DEVICES 3

SPI spi0(SPI0_MOSI, SPI0_MISO, SPI0_SCK); // 0, 1
DigitalOut cs0(SPI0_CS);


static int gBitRate[SPI_MAX_DEVICES] = {0};
static char gOffsetAddress[SPI_MAX_DEVICES] = {N25Q032A_CMD_READ_ID,N25Q032A_CMD_READ_ID,0x00};

static void cs_init(void) {
	cs0=1;
}

static int BitRate(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len, gBitRate[index] == 0 ? " (1MHz)" : " (100KHz)");
		return 0;
	}
	console_printf("\r\n*) Please input the bitrate ( 0: 1MHz , 1: 100KHz ): \r\n");
	gBitRate[index] = GetInt_wait();
    
    if(gBitRate[index] < 0 || gBitRate[index] > 1) gBitRate[index] = 0;
	return 0;
}

static int OffsetAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gOffsetAddress[index]);
		return 0;
	}
	console_printf("\r\n*) Please input the offset address ( 0x00 ~ 0xFF ): \r\n");
	gOffsetAddress[index] = (char)(GetInt_wait()&0xFF);
    
    if(gOffsetAddress[index] < 0 || gOffsetAddress[index] > 0xFF) gOffsetAddress[index] = N25Q032A_CMD_READ_ID;
	return 0;
}

static Mutex spi_mutex;
static void ReadComplete(int ev) {
    spi_mutex.unlock();
}

static int Read(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    SPI *spi;
	DigitalOut *cs;
    
    switch(index) {
        case 0:
            spi = &spi0;
			cs = &cs0;
        break;
        
        case 1:
            //spi = &spi1;
            // cs = &cs1;
        break;
        
        case 2:
			//spi = &spirf;
			//cs = &csrf;
        break;

        default:
			console_printf("Default SPI test.\r\n");
            spi = &spi0;
			cs = &cs0;
        break;			
    }
    

	spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0x00};
    char rx[5] = {0x00};
    
   	*cs = 0;
    rx[0] = spi->write(gOffsetAddress[index]);
    rx[1] = spi->write(0x00);
    rx[2] = spi->write(0x00);
    rx[3] = spi->write(0x00);
    rx[4] = spi->write(0x00);
	*cs = 1;

	console_printf("\r\n");
	console_printf("Result: 0x%08X 0x%08X 0x%08X 0x%08X\n",rx[1],rx[2],rx[3],rx[4]);
	console_printf("\r\n");
    
	return true;
}

int SPITest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    if(index < 0 || index >= SPI_MAX_DEVICES) return 0;
    
	simple_cmds cmds[] = {
            { "SPI Test" /*title*/,  					NULL,NULL },
            { "BitRate",							    index,BitRate },
            { "Offset Address",							index,OffsetAddress },
            { "Read",			                        index,Read },
            { NULL,										NULL,NULL }
    };
	cs_init();
	return simple_cli_list(cmds);
}

