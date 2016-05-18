#include "ads7843.h"

Point::Point(void) {
  x = y = 0;
}

Point::Point(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool Point::operator==(Point p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool Point::operator!=(Point p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}


ADS7843 :: ADS7843(uint32_t cs, uint32_t dclk, uint32_t din, uint32_t dout, uint32_t irq)
{
	_cs = cs;
	_dclk = dclk;
	_din = din;
	_dout = dout;
	_irq = irq;

	_cs_port = digitalPinToPort(_cs);
	_dclk_port = digitalPinToPort(_dclk);
	_din_port = digitalPinToPort(_din);
	_dout_port = digitalPinToPort(_dout);
	_irq_port = digitalPinToPort(_irq);

	_cs_pin = digitalPinToBitMask(_cs);
	_dclk_pin = digitalPinToBitMask(_dclk);
	_din_pin = digitalPinToBitMask(_din);
	_dout_pin = digitalPinToBitMask(_dout);
	_irq_pin = digitalPinToBitMask(_irq);

}

void ADS7843 :: begin()
{
	pinMode(_cs, OUTPUT);
	pinMode(_dclk, OUTPUT);
	pinMode(_din, OUTPUT);
	pinMode(_dout, INPUT);
	pinMode(_irq, INPUT);

	digitalWrite(_cs, HIGH);
	digitalWrite(_dclk, HIGH);
	digitalWrite(_din, HIGH);
}

void ADS7843 :: write(uint8_t dat)
{
	uint8_t nop;
	digitalWrite(_dclk,LOW);
	for(int i=0; i<8; i++) {
		if(dat&0x80)
			digitalWrite(_din,HIGH);
		else
			digitalWrite(_din,LOW);

		dat=dat<<1;

		digitalWrite(_dclk,LOW);
		nop++;
		nop++;
		digitalWrite(_dclk,HIGH);
		nop++;
		nop++;
	}
}

uint16_t ADS7843 :: read()
{
	uint8_t nop;
	uint16_t Num=0;
	for(int i=0; i<12; i++) {
		Num<<=1;
		digitalWrite(_dclk,HIGH);//DCLK=1; _nop_();_nop_();_nop_();
		nop++;
		digitalWrite(_dclk,LOW);//DCLK=0; _nop_();_nop_();_nop_();
		nop++;
		if(digitalRead(_dout)) 
			Num++;
	}
	return(Num);
}
#define ADS7843_READ_TIMES		10
#define ADS7843_LOST_VAL		3
Point ADS7843 :: getpos(uint8_t *flag)
{
	Point p[ADS7843_READ_TIMES];
	Point P;
	int i;
	int k,j,x_tmp, y_tmp;
	uint32_t sum_x, sum_y;
	uint16_t tmp;
	
	i=0; sum_x = 0; sum_y = 0;
	
	while(check_irq()){
		
		getPosOnce( (uint16_t *)(&(p[i].x)), (uint16_t *)(&(p[i].y)) );
		
		i++;
		if(i==ADS7843_READ_TIMES){
			/** sort here */
			
			for(k=0; k<ADS7843_READ_TIMES-1; k++){
				for(j=k+1; j<=ADS7843_READ_TIMES; j++){
					if(p[k].x > p[j].x){
						x_tmp = p[j].x;
						p[j].x=p[k].x;
						p[k].x=x_tmp;
					}
					if(p[k].y > p[j].y){
						y_tmp = p[j].y;
						p[j].y=p[k].y;
						p[k].y=y_tmp;
					}
				}
			}

#if ADS7843_DEBUG
			for(k=0; k<ADS7843_READ_TIMES; k++){
				Serial.print(p[k].x);
				Serial.print(" ");
				Serial.print(p[k].y);
				Serial.print(" ");
			}
			Serial.println(" ");
#endif

			for(k=ADS7843_LOST_VAL; k<(ADS7843_READ_TIMES-ADS7843_LOST_VAL); k++){
				sum_x += (uint32_t)p[k].x;
				sum_y += (uint32_t)p[k].y;
			}
			
			sum_x /= (ADS7843_READ_TIMES - 2*ADS7843_LOST_VAL);
			sum_y /= (ADS7843_READ_TIMES - 2*ADS7843_LOST_VAL);
			
#if ADS7843_DEBUG
			Serial.print("SUM:");
			Serial.print(sum_x, DEC);
			Serial.print("   ");
			Serial.print(sum_y, DEC);
			Serial.println();
#endif
			P.x = (uint16_t)sum_x;
			P.y = (uint16_t)sum_y;;

#if ADS7843_DEBUG
			Serial.print("I:");
			Serial.print(P.x, DEC);
			Serial.print("   ");
			Serial.print(P.y, DEC);
			Serial.println();
			Serial.print("C:");
#endif
			*flag = 1;
			return P;
		}
	}
	*flag = 0;
	return P;
}

void ADS7843 :: getPosOnce(uint16_t *x, uint16_t *y)
{
	digitalWrite(_cs,LOW);                    
	write(0x90); 
	digitalWrite(_dclk,HIGH);
	digitalWrite(_dclk,LOW); 
	*y=read();
	write(0xD0);   
	digitalWrite(_dclk,HIGH);
	digitalWrite(_dclk,LOW);
	*x=read();
	digitalWrite(_cs,HIGH);
}

uint8_t ADS7843 :: check_irq(void)
{
	return ( (digitalRead(_irq)==HIGH) ? 0 : 1 );
}

uint8_t ADS7843 :: wr(uint8_t dat)
{
	

}