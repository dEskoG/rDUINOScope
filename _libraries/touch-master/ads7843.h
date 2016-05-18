#ifndef __ADS7843_H
#define __ADS7843_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

class Point {
 public:
  Point(void);
  Point(int16_t x, int16_t y, int16_t z);
  
  bool operator==(Point);
  bool operator!=(Point);

  int16_t x, y, z;
};

class ADS7843{
public:
	ADS7843(uint32_t cs, uint32_t dclk, uint32_t din, uint32_t dout, uint32_t irq);
	void begin();
	void write(uint8_t dat);
	uint16_t read();
	Point getpos(uint8_t *flag);
	void getPosOnce(uint16_t *x, uint16_t *y);
	uint8_t check_irq(void);
	uint8_t wr(uint8_t dat);
private:
	uint32_t _cs, _dclk, _din, _dout, _irq;
	Pio* _cs_port, *_dclk_port, *_din_port, *_dout_port, *_irq_port;
	uint32_t _cs_pin, _dclk_pin, _din_pin, _dout_pin, _irq_pin;
};

#endif