#include "tftlib.h"

#include "star.c"

#define TFTWIDTH   240
#define TFTHEIGHT  400

TFTLCD::TFTLCD()
{
#ifdef ELECFREAKS_TFT_SHIELD_V2
	for(int p=22; p<42; p++) {
		pinMode(p,OUTPUT);
	}
	for(int p=2; p<7; p++) {
		pinMode(p,OUTPUT);
	}
#elif defined ELECHOUSE_DUE_TFT_SHIELD_V1
	for(int p=34; p<42; p++) {
		pinMode(p,OUTPUT);
	}
	for(int p=44; p<52; p++) {
		pinMode(p,OUTPUT);
	}
	
	pinMode(23,OUTPUT);
	pinMode(24,OUTPUT);
	pinMode(22,OUTPUT);
	pinMode(31,OUTPUT);
	pinMode(33,OUTPUT);
	
#else
#error "initial io here"
#endif
	DB_WR_EN();
	RD_IDLE;
}

void TFTLCD::begin()
{
	constructor(TFTWIDTH, TFTHEIGHT);

	reset();
	CS_ACTIVE;
	writeRegister16(0x0083, 0x0002);
	writeRegister16(0x0085, 0x0003);
	writeRegister16(0x008B, 0x0001);
	writeRegister16(0x008C, 0x0093);
	writeRegister16(0x0091, 0x0001);
	writeRegister16(0x0083, 0x0000);
	writeRegister16(0x003E, 0x00B0);
	writeRegister16(0x003F, 0x0003);
	writeRegister16(0x0040, 0x0010);
	writeRegister16(0x0041, 0x0056);
	writeRegister16(0x0042, 0x0013);
	writeRegister16(0x0043, 0x0046);
	writeRegister16(0x0044, 0x0023);
	writeRegister16(0x0045, 0x0076);
	writeRegister16(0x0046, 0x0000);
	writeRegister16(0x0047, 0x005E);
	writeRegister16(0x0048, 0x004F);
	writeRegister16(0x0049, 0x0040);

	writeRegister16(0x0017, 0x0091);
	writeRegister16(0x002B, 0x00F9);
	delay(10);

	writeRegister16(0x001B, 0x0014);
	writeRegister16(0x001A, 0x0011);
	writeRegister16(0x001C, 0x0006);
	writeRegister16(0x001F, 0x0042);
	delay(20);

	writeRegister16(0x0019, 0x000A);
	writeRegister16(0x0019, 0x001A);
	delay(40);

	writeRegister16(0x0019, 0x0012);
	delay(40);

	writeRegister16(0x001E, 0x0027);
	delay(100);

	writeRegister16(0x0024, 0x0060);
	writeRegister16(0x003D, 0x0040);
	writeRegister16(0x0034, 0x0038);
	writeRegister16(0x0035, 0x0038);
	writeRegister16(0x0024, 0x0038);
	delay(40);

	writeRegister16(0x0024, 0x003C);

	writeRegister16(0x0016, 0x001C);

	writeRegister16(0x0001, 0x0006);
	writeRegister16(0x0055, 0x0000);

	writeRegister16(0x0002, 0x0000);
	writeRegister16(0x0003, 0x0000);
	writeRegister16(0x0004, 0x0000);
	writeRegister16(0x0005, 0x00ef);
	writeRegister16(0x0006, 0x0000);
	writeRegister16(0x0007, 0x0000);
	writeRegister16(0x0008, 0x0001);
	writeRegister16(0x0009, 0x008f);

	CS_IDLE;

	rotation = 0;
}

void TFTLCD::writeRegister16(uint16_t reg, uint16_t dat)
{
	CD_COMMAND;
	write16(reg);
	CD_DATA;
	write16(dat);
}

void TFTLCD::writeCommand16(uint16_t cmd)
{
	CD_COMMAND;
	write16(cmd);
}

void TFTLCD::writeData16(uint16_t dat)
{
	CD_DATA;
	write16(dat);
}

void TFTLCD::setRotation(uint8_t x)
{
	CS_ACTIVE;
    Adafruit_GFX::setRotation(x);
#if 1
	switch(x) {
	case 0://All zero
	default:
		/**
			START: top left. END bottom right.
			DIR: top left --> top right
		*/
		writeRegister16(0x0016, 0x001C);
		break;
	case 1://MV = 0, MX = 1, MY = 0
		/**
			START: top right. END bottom left.
			DIR: top right --> top left
		*/
		writeRegister16(0x0016, 0x005C);
		break;
	case 3://MV = 0, MX = 0, MY = 1
		/**
			START: bottom left. END top right.
			DIR: bottom left --> bottom right
		*/
		writeRegister16(0x0016, 0x009C);
		break;
		break;
	case 2://MV = 0, MX = 1, MY = 1
		/**
			START: bottom right.  END top left.
			DIR: bottom right --> bottom left
		*/
		writeRegister16(0x0016, 0x00DC);
		break;
	}
#else
    switch(x) {
	case 0://All zero
	default:
		/**
			START: top left. END bottom right.
			DIR: top left --> top right
		*/
		rotation = 0;
		writeRegister16(0x0016, 0x001C);
		break;
	case 1://MV = 1, MX = 0, MY = 0
		/**
			START: top left. END bottom right.
			DIR: top left --> bottom left
		*/
		writeRegister16(0x0016, 0x003C);
		break;
	case 2://MV = 0, MX = 1, MY = 0
		/**
			START: top right. END bottom left.
			DIR: top right --> top left
		*/
		writeRegister16(0x0016, 0x005C);
		break;
	case 3://MV = 1, MX = 1, MY = 0
		/**
			START: top right. END bottom right.
			DIR: top right --> bottom right
		*/
		writeRegister16(0x0016, 0x007C);
		break;
	case 4://MV = 0, MX = 0, MY = 1
		/**
			START: bottom left. END top right.
			DIR: bottom left --> bottom right
		*/
		writeRegister16(0x0016, 0x009C);
		break;
	case 5://MV = 1, MX = 0, MY = 1
		/**
			START: bottom left. END top right.
			DIR: bottom left --> top left
		*/
		writeRegister16(0x0016, 0x00BC);
		break;
	case 6://MV = 0, MX = 1, MY = 1
		/**
			START: bottom right.  END top left.
			DIR: bottom right --> bottom left
		*/
		writeRegister16(0x0016, 0x00DC);
		break;
	case 7://MV = 1, MX = 1, MY = 1,
		/**
			START: bottom right.  END top left.
			DIR: bottom right --> top right.
		*/
		writeRegister16(0x0016, 0x00FC);
		break;
	}
#endif
	CS_IDLE;
}


void TFTLCD::setAddrWindow(int x1, int y1, int x2, int y2)
{
	int x_b, y_b, x_e, y_e;
#if 1
    switch(getRotation()) {
	case 0:
	default:
		x_b = x1;
		y_b = y1;
		x_e = x2;
		y_e = y2;
		break;
	case 1:
        x_b = y1;
		y_b = x1;
		x_e = y2;
		y_e = x2;
		break;
	case 3:
		x_b = y1;
		y_b = x1;
		x_e = y2;
		y_e = x2;
		break;
	case 2:
        x_b = x1;
		y_b = y1;
		x_e = x2;
		y_e = y2;
		break;
	}

#else
	switch(rotation) {
	case 0:
	default:
		x_b = x1;
		y_b = y1;
		x_e = x2;
		y_e = y2;
		break;
	case 1:
		x_b = y1;
		y_b = x1;
		x_e = y2;
		y_e = x2;
		break;
	case 2:
		x_b = TFTWIDTH - 1 - x2;
		y_b = y1;
		x_e = TFTWIDTH - 1 - x1;
		y_e = y2;
		break;
	case 3:
		x_b = y1;
		y_b = TFTWIDTH -1 - x2;
		x_e = y2;
		y_e = TFTWIDTH -1 - x1;
		break;
	case 4:
        x_b = x1;
		y_b = TFTHEIGHT - 1 - y2;
		x_e = x2;
		y_e = TFTHEIGHT - 1 - y1;
		break;
	case 5:
		x_b = TFTHEIGHT - 1 - y2;
		y_b = x1;
		x_e = TFTHEIGHT - 1 - y1;
		y_e = x2;
		break;
	case 6:
		x_b = TFTWIDTH -1 - x2;
		y_b = TFTHEIGHT - 1 - y2;
		x_e = TFTWIDTH -1 - x1;
		y_e = TFTHEIGHT - 1 - y1;
		break;
	case 7:
		x_b = TFTHEIGHT - 1 - y2;
		y_b = TFTWIDTH -1 - x2;
		x_e = TFTHEIGHT - 1 - y1;
		y_e = TFTWIDTH -1 - x1;
		break;
	}
#endif
	CS_ACTIVE;
	writeRegister16(0x0002,x_b>>8);
	writeRegister16(0x0003,x_b);
	writeRegister16(0x0006,y_b>>8);
	writeRegister16(0x0007,y_b);
	writeRegister16(0x0004,x_e>>8);
	writeRegister16(0x0005,x_e);
	writeRegister16(0x0008,y_e>>8);
	writeRegister16(0x0009,y_e);
	CS_IDLE;
}

void TFTLCD::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	setAddrWindow(x, y, x, y);
	CS_ACTIVE;
	writeCommand16(0x0022);
	writeData16(color);
	CS_IDLE;
}

void TFTLCD::reset()
{
	CS_IDLE;
	CD_DATA;
	WR_IDLE;
	RD_IDLE;

	RST_IDLE;
	delay(5);
	RST_ACTIVE;
	delay(15);
	RST_IDLE;
	delay(15);
}

void TFTLCD::flood(uint16_t color, uint32_t len)
{
	CS_ACTIVE;
	writeCommand16(0x0022);
	CD_DATA;
	for(int i=0; i<len; i++) {
		write16(color);
	}
	CS_IDLE;
}

void TFTLCD::fillScreen(uint16_t color)
{
	setAddrWindow(0, 0, width() - 1, height() -  1);
    flood(color, (long)width()*(long)height());
}

void TFTLCD::pushColors(uint16_t *data, uint8_t len, boolean first)
{
	CS_ACTIVE;
	if(first == true) {
		writeCommand16(0x0022);
	}
	CD_DATA;
	for(int i=0; i<len; i++) {
		write16(data[i]);
	}
	CS_IDLE;
}

void TFTLCD::drawPic(uint8_t *data, uint32_t len)
{
	uint16_t color;
//	data = (uint8_t *)gImage_star;
	CS_ACTIVE;
	writeCommand16(0x0022);
	CD_DATA;
	for(int i=0; i<len; i++) {
		color = ((uint16_t)(*data))|(((uint16_t)(*(data+1)))<<8);
		write16(color);
		data+=2;
	}
	CS_IDLE;
}

uint16_t TFTLCD::color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}