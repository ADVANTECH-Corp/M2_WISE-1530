#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "I2CTest.h"

#define console_printf ((Serial *)printer)->printf


//HDC1050 Slave
#define TI_HDC1050_DEVICE_ADDR  0x40
#define NXP_PCA9545APW_DEV_ADDR  0x70


//HDC1050 Offset
#define TI_HDC1050_MANUFACTURER_ID_ADDR	0xFE

static int gBitRate = 0;
static unsigned char gSlaveAddress = TI_HDC1050_DEVICE_ADDR;
static unsigned char gOffsetAddress = TI_HDC1050_MANUFACTURER_ID_ADDR;

static char gChannel = 0;
static char gWriteData[4] = {0};


I2C Myi2c(I2C0_SDA, I2C0_SCL);

static int BitRate(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (%dKHz)",gBitRate == 0 ? 100 : 400);
		return 0;
	}
	console_printf("\r\n*) Please input the bitrate ( 0: 100KHz, 1: 400KHz ): \r\n");
	gBitRate = GetInt_wait();
    
    if(gBitRate < 0 || gBitRate > 1) gBitRate = 0;
	return 0;
}

static int SlaveAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gSlaveAddress);
		return 0;
	}
	console_printf("\r\n*) Please input the slave address ( 0x00 ~ 0x7F ): \r\n");
	gSlaveAddress = (unsigned char)(GetInt_wait()&0xFF);
	return 0;
}


static int OffsetAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gOffsetAddress);
		return 0;
	}
	console_printf("\r\n*) Please input the offset address ( 0x00 ~ 0xFF ): \r\n");
	gOffsetAddress = (unsigned char)(GetInt_wait()&0xFF);
	return 0;
}

static void I2C_Read(I2C *i2c, char slave, char offset, char *rx, int rxlen) {
	i2c->write(slave<<1, &offset, 1);
   	i2c->read(slave<<1, rx, rxlen);
}

static int Read_HDC105_ID(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0)
		return 0;
	
    I2C *i2c = &Myi2c;
    switch(gBitRate) {
        case 1:
            i2c->frequency(400000);
        break;
        default:
            i2c->frequency(100000);
        break;
    }
    char buffer[2] = {0};
    console_printf("\r\n");
    console_printf("Read from 0x%02X--0x%02X\r\n",gSlaveAddress, gOffsetAddress);
	I2C_Read(i2c, gSlaveAddress, gOffsetAddress, buffer, sizeof(buffer));
	console_printf("Result: 0x%02X%02X\r\n",buffer[0],buffer[1]);
	console_printf("\r\n");
	return true;
}

static int I2C_SW_Sel(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," Channel:(%d)", gChannel);
		return 0;
	}
	console_printf("\r\n*) Please input channel ( 0 ~ 3 ): \r\n");
	gChannel = (unsigned char)(GetInt_wait()&0xFF);
	return 0;
}

static int Set_I2C_SW(int index, char *string, int len, void *printer) {

	I2C *i2c = &Myi2c;
	char data;

	if(string != NULL && len != 0){
		return 0;	
	}

    switch(gBitRate) {
        case 1:
            i2c->frequency(400000);
        break;
        default:
            i2c->frequency(100000);
        break;
    }
	
	I2C_Read(i2c, NXP_PCA9545APW_DEV_ADDR, (0x01<<gChannel), &data, 1);
	if((data & 0x0f)==(0x01<<gChannel)){
		console_printf("Set channel to %d\r\n", gChannel);
	}
	else{
		console_printf("Set channel error\r\n");
	}

	return true;
}


static void EEP_24C32_Wr(I2C *i2c, char slave, char offset, char *tx, int txlen) {
	char ofs_2byte[2]={0x0, offset};
	
	i2c->write(slave<<1, ofs_2byte, 2, true);
   	i2c->write(slave<<1, tx, txlen, false);
}


static int EEP_24C32_Write(int index, char *string, int len, void *printer) {
	char data[INBUF_SIZE];
	unsigned int leng;
	int inx=0;
	int SPos;
	unsigned long Val;

	if(string != NULL && len != 0){
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)(0x%02X)(0x%02X)(0x%02X)",
											 gWriteData[0],
											 gWriteData[1],
											 gWriteData[2],
											 gWriteData[3]);
		return 0;
	}
	console_printf("\r\n*) Please input 4 bytes for Write data ( 0x00 ~ 0xFF ): \r\n");
	if(GetStr_wait(data, &leng)!=0){		
		do{		
			leng=strlen(data);
			if(leng==0)
				break;
			SPos=FindSpace(data);		
			Val=strtoul(data, NULL, 0);
			gWriteData[inx++]= (Val>0xFF) ? 0xFF : Val;
			if(SPos)
				memcpy(data, &data[SPos+1], (leng-SPos));
			else
				data[0]='\0';

			wait_us(200);

		}while ((data[0]=='\0') || (inx<sizeof(gWriteData)));
		
	}

    I2C *i2c = &Myi2c;
    switch(gBitRate) {
        case 1:
            i2c->frequency(400000);
        break;
        default:
            i2c->frequency(100000);
        break;
    }	

	EEP_24C32_Wr(i2c, gSlaveAddress, gOffsetAddress, gWriteData, sizeof(gWriteData));

	return 0;
}

static void EEP_24C32_Rd(I2C *i2c, char slave, char offset, char *rx, int rxlen) {
	char ofs_2byte[2]={0x0, offset};

	i2c->write(slave<<1, ofs_2byte, 2);
   	i2c->read(slave<<1, rx, rxlen);
}


static int EEP_24C32_Read(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0)
		return 0;
	
    I2C *i2c = &Myi2c;
    switch(gBitRate) {
        case 1:
            i2c->frequency(400000);
        break;
        default:
            i2c->frequency(100000);
        break;
    }
    console_printf("\r\n");
    console_printf("Read from DevID:0x%02X, Offset:0x%02X\r\n",gSlaveAddress, gOffsetAddress);
	EEP_24C32_Rd(i2c, gSlaveAddress, gOffsetAddress, gWriteData, 4);
	console_printf("Result: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",gWriteData[0],gWriteData[1],gWriteData[2],gWriteData[3]);
	console_printf("\r\n");
	return true;
}


int I2CTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "I2C Test" /*title*/,  					    0, NULL },
            { "BitRate",							    index, BitRate },
            { "Slave Address",							index, SlaveAddress },
            { "Offset Address",							index, OffsetAddress },
            { "Read TI_HDC105 ID",			        	index, Read_HDC105_ID },
			{ "I2C-SW Channel Select",			        index, I2C_SW_Sel },
			{ "Set I2C-SW Channel",				        index, Set_I2C_SW },
            { "EEP_24C32_Write",			            index, EEP_24C32_Write },
            { "EEP_24C32_Read",			                index, EEP_24C32_Read },
            { NULL,										0, NULL }
    };
	return simple_cli_list(cmds);
}

