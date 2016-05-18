//rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.// SCREEN 47 mm x 80 mm - screen 240 x 400 - 3.2'

// HERE GOES THE Mount, Gears and Drive information.
// ... used to calculate the HourAngle to microSteps ratio
// UPDATE THIS PART according to your SET-UP
// ---------------------------------------------
// NB: RA and DEC uses the same gear ratio (144 tooth in my case)!
//----------------------------------------------
int WORM = 144;
int REDUCTOR = 4;  // 1:4 gear reduction
int DRIVE_STP = 200;   // Stepper drive have 200 steps per revolution
int MICROSteps = 16;   // I'll use 1/16 microsteps mode to drive sidereal - also determines the LOWEST speed.

// below variables are used to calculate the paramters where the drive works
int ARCSEC_F_ROTAT = 1296000;    // ArcSeconds in a Full earth rotation;
float SIDEREAL_DAY = 86164.0905;   // Sidereal day in seconds
float ArcSECstep;
int MicroSteps_360;
int RA_90;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
int DEC_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
int HA_H_CONST;
int HA_M_CONST;
int DEC_D_CONST;
int DEC_M_CONST;
int MIN_TO_MERIDIAN_FLIP = 2;   // This constant tells the system when to do the Meridian Flip. "= 2" means 2 minutes before 24:00h (e.g. 23:58h)
int MIN_SOUND_BEFORE_FLIP = 3;   // This constant tells the system to start Sound before it makes Meridian Flip
float mer_flp;                   // The calculateLST_HA() function depending on this timer will convert the HA and DEC to the propper ones to do the flip.
boolean MERIDIAN_FLIP_DO = false;
int Clock_Motor;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds

////////////////////////////////////////////////
#include "DHT.h"
#include <TinyGPS++.h>
#include <Time.h>
#include <ads7843.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <tftlib.h> // Hardware-specific library
#include <DueTimer.h> // interruptions library
#include <DS3231.h>
#include <math.h>

//#define PSTR(a)  a
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

/** ADS7843 pin map */
#ifdef ELECFREAKS_TFT_SHIELD_V2
#define DCLK     6
#define CS       5  
#define DIN      4 
#define DOUT     3
#define IRQ      2 
#elif defined ELECHOUSE_DUE_TFT_SHIELD_V1
/** elechouse TFT shield pin map */
#define DCLK     25
#define CS       26 
#define DIN      27 
#define DOUT     29
#define IRQ      30
#endif
// Assign human-readable names to some common 16-bit color values:
#define BLACK           0x0000      /*   0,   0,   0 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define GREEN           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F
#define DHTPIN 3
#define DHTTYPE DHT22
#define  note_c     3830    // 261 Hz 
#define  note_cb    3610    // 261 Hz 
#define  note_d     3400    // 294 Hz 
#define  note_e     3038    // 329 Hz 
#define  note_f     2864    // 349 Hz 
#define  note_g     2550    // 392 Hz 
#define  note_gb    2411    // 392 Hz 
#define  note_a     2272    // 440 Hz 
#define  note_b     2028    // 493 Hz 
#define  note_C     1912    // 523 Hz 
#define  note_X     1650    // 
//
//..... Below are the VARs for teh code. Most of them are self explenatory
//
DHT dht(DHTPIN, DHTTYPE);
ADS7843 touch(CS, DCLK, DIN, DOUT, IRQ);
Point p;
TFTLCD tft;
TinyGPSPlus gps;
DS3231 rtc(A4, A5); // (SDA, SCL) from the RTC board
int sd_cs = 42;              // TFTShield sd card chip select pin is Arduino digital pin 42
String Messier_Array[120];
String Treasure_Array[130];
String ObservedObjects[50];
String Stars[] = {
"Aql;Altair;19h50.7935m;8°52.205m",
"Ari;Hamal;02h7.1757m;23°27.7317m",
"Aur;Capella;05h16.693m;45°59.7583m",
"Aur;Menkalinan;05h59.5273m;44°56.845m",
"Boo;Arcturus;14h15.6426m;19°10.346m",
"Car;Canopus;06h23.9525m;-52°41.7317m",
"Car;Miaplacidus;09h13m;-69.7°m",
"Car;Avior;08h23m;-59.5°m",
"Cen;Rigil Kent.;14h40m;-60.8°m",
"Cen;Hadar;14h04m;-60.4°m",
"CMa;Sirius;06h45m;-16.7°m",
"CMa;Adhara;06h59m;-29.0°m",
"CMa;Wezen;07h08m;-26.4°m",
"CMa;Mirzam;06h23m;-18.0°m",
"CMi;Procyon;07h39m;+5.2°m",
"Cru;Acrux;12h27m;-63.1°m",
"Cru;Mimosa;12h48m;-59.7°m",
"Cru;Gacrux;12h31m;-57.1°m",
"Cyg;Deneb;20h41m;+45.3°m",
"Eri;Achernar;01h38m;-57.2°m",
"Gem;Pollux;07h45m;+28.0°m",
"Gem;Castor;07h35m;+31.9°m",
"Gem;Alhena;06h38m;+16.4°m",
"Gru;Alnair;22h08m;-47.0°m",
"Hya;Alphard;09h28m;-8.7°m",
"Leo;Regulus;10h08m;+12.0°m",
"Leo;Algieba;10h20m;+19.8°m",
"Lyr;Vega;18h37m;+38.8°m",
"Ori;Rigel;05h15m;-8.2°m",
"Ori;Betelgeuse;05h55m;+7.4°m",
"Ori;Bellatrix;05h25m;+6.3°m",
"Ori;Alnilam;05h36m;-1.2°m",
"Ori;Alnitak;05h41m;-1.9°m",
"Pav;Peacock;20h26m;-56.7°m",
"Per;Mirfak;03h24m;+49.9°m",
"PsA;Fomalhaut;22h58m;-29.6°m",
"Sco;Antares;16h29m;-26.4°m",
"Sco;Shaula;17h34m;-37.1°m",
"Sco;Sargas;17h37m;-43.0°m",
"Sgr;Kaus Aust.;18h24m;-34.4°m",
"Tau;Aldebaran;04h36m;+16.5°m",
"Tau;Elnath;05h26m;+28.6°m",
"TrA;Atria;16h49m;-69.0°m",
"UMa;Alioth;12h54m;+56.0°m",
"UMa;Dubhe;11h04m;+61.8°m",
"UMa;Alkaid;13h48m;+49.3°m",
"UMi;Polaris;02h32m;+89.3°m",
"Vel;Regor;08h10m;-47.3°m",
"Vel;Koo She;08h45m;-54.7°m",
"Vir;Spica;13h25m;-11.2°m"
};
int Observed_Obj_Count = 0;
String Prev_Obj_Start;
int lastScreenUpdateTimer;
int Slew_timer, Slew_RA_timer = 0;
int OBJECT_Index;
String OBJECT_NAME;
String OBJECT_DESCR;
String OBJECT_DETAILS;
String BT_COMMAND_STR;
String START_TIME;
int STP_FWD = HIGH;
int STP_BACK = LOW;
float OBJECT_RA_H;
float OBJECT_RA_M;
float OBJECT_DEC_D;
float OBJECT_DEC_M;
float OBJECT_MAG;
int SELECTED_STAR = 0;
float DELTA_RA_ADJUST = 1; // cos RA
float DELTA_DEC_ADJUST = 1; // cos DEC
// Default values to load when CANCEL button is hit on the GPS screen
float OBSERVATION_LONGITUDE = 23.6380; // (23.3333* - Home)
float OBSERVATION_LATTITUDE = 42.6378; // (42.6378* - Home)
float OBSERVATION_ALTITUDE = 760.60; // Sofia, Bulgaria
int TIME_ZONE = 2;
// .............................................................
int GPS_iterrations = 0;
double LST, HAHour, HAMin, ALT;
double JD;
int last_button, MESS_PAGER, TREAS_PAGER, STARS_PAGER;
boolean IS_OBJ_VISIBLE = false;
boolean IS_IN_OPERATION = false;  // This variable becomes True when Main screen appears
boolean IS_TRACKING = false;
boolean IS_NIGHTMODE;
boolean IS_OBJ_FOUND = true;
boolean IS_OBJECT_RA_FOUND = true;
boolean IS_OBJECT_DEC_FOUND = true;
boolean IS_MERIDIAN_PASSED = false;
boolean IS_BT_MODE_ON = false;
boolean IS_MANUAL_MOVE = false;
int CURRENT_SCREEN = 0; 
String old_t, old_d;
String Start_date;
int update_time, Tupdate_time;
int w_DateTime[12]={0,0,0,0,0,0,0,0,0,0,0,0};  // array to store date - as updated from updater screen - Wishing_Date
int dateEntryPos = 0;
int Summer_Time = 0;
int xPosition = 0;  // Joystick
int yPosition = 0;  // Joystick
float _temp,_Stemp;
float _humid,_Shumid;
int16_t texts, l_text, d_text, btn_l_border, btn_d_border, btn_l_text, btn_d_text, btn_l_selection, title_bg, title_texts, messie_btn, file_btn, ngc_btn, MsgBox_bg, MsgBox_t;    // defines string constants for the clor - Depending on the DAY/NIGHT modes
int LOAD_SELECTOR;   // selector to show which LOADING mechanism is used: 1 - Messier, 2 - File, 3 - NGCs
File roots;
int RA_microSteps, DEC_microSteps;
int RA_mode_steps, DEC_mode_steps;
int SLEW_RA_microsteps, SLEW_DEC_microsteps;
int RA_finish_last = 0;

// PIN selection
int speakerOut = 2;
//int dht_pin = 3;
int RA_STP = 4;
int RA_DIR = 5;
int DEC_STP = 6;
int DEC_DIR = 7;

int RA_MODE1 = 13;
int RA_MODE2 = 12;
int RA_MODE0 = 11;     
int DEC_MODE0 = 10;     
int DEC_MODE1 = 9;
int DEC_MODE2 = 8;
// RTC (A4, A5); // (SDA, SCL) from the RTC board
// 17 (RX) - goes to TX on GPS;
// 16 (TX) - goes to RX on GPS;
// 15 (RX) - goes to Bluetooth RX;
// 14 (TX) - goes to Bluetooth TX;
int xPin = A1;
int yPin = A0;


void setup(void) {
  Serial.begin(9600);
  Serial2.begin(9600);  // Initialize GPS communication on PINs: 17 (RX) and 16 (TX) 
  Serial3.begin(9600); // Bluetooth communication on PINs:  15 (RX) and 14 (TX)  
  pinMode(speakerOut, OUTPUT);

  // below variables are used to calculate the paramters where the drive works
  int ww = WORM*REDUCTOR;
  int www = DRIVE_STP*MICROSteps;
  MicroSteps_360 = ww*www;
  RA_90 = MicroSteps_360 / 4;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
  DEC_90 = RA_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
  HA_H_CONST = MicroSteps_360/360;
  DEC_D_CONST = HA_H_CONST;
  Clock_Motor = 1000000/(MicroSteps_360/SIDEREAL_DAY);  // This way I make the interruption occuer 2wice faster than needed - REASON: allow max time for Pin-UP, Pin-DOWN action
  //Serial.println(MicroSteps_360);
  //Serial.println(ARCSEC_F_ROTAT);
  //Serial.println(RA_90);
  //Serial.println(DEC_90);
  //Serial.println(ArcSECstep,5);
  //Serial.println(HA_H_CONST);
  //Serial.println(HA_M_CONST);
  //Serial.println(DEC_D_CONST);
  //Serial.println(DEC_M_CONST);
  //Serial.println(Clock_Motor);
  touch.begin();
  rtc.begin();
  dht.begin();

  tft.begin();
  tft.fillScreen(BLACK);
  tft.setCursor(15, 10);

  // DRB8825 - drive mode pins (determine Steppping Modes 1/8, 1/16 and etc.
  pinMode(RA_MODE0, OUTPUT); 
  pinMode(RA_MODE1, OUTPUT); 
  pinMode(RA_MODE2, OUTPUT);

  pinMode(DEC_MODE0, OUTPUT); 
  pinMode(DEC_MODE1, OUTPUT); 
  pinMode(DEC_MODE2, OUTPUT);

  // Joystick
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  // Set RA and DEC microstep position 
  RA_microSteps = RA_90; //  --> point to North Sudereal Pole = -180 deg (-12h)
  DEC_microSteps = 0; //  --> Point to North Sudereal Pole = 90 deg
   
  Timer3.attachInterrupt(Sidereal_rate);
//  Timer3.start(Clock_Motor); // executes the code every 62.329 ms.

  pinMode(RA_STP,OUTPUT); // Step
  pinMode(RA_DIR,OUTPUT); // Dir
  digitalWrite(RA_DIR,HIGH); // Set Dir high
  digitalWrite(RA_STP,LOW);
  
  pinMode(DEC_STP,OUTPUT); // Step
  pinMode(DEC_DIR,OUTPUT); // Dir
  digitalWrite(DEC_DIR,HIGH); // Set Dir high
  digitalWrite(DEC_STP,LOW);

  if (analogRead(A3) < 800){
    IS_NIGHTMODE = true;
  } else {
    IS_NIGHTMODE = false;
  }
  considerDayNightMode();

  // Draw initial screen - INITIALIZE
  tft.setTextColor(title_bg);    
  tft.setTextSize(3);
  tft.println("rDUINO SCOPE");
  tft.setTextColor(l_text);    
  tft.setTextSize(2);
  tft.setCursor(35, 35);
  tft.setTextColor(l_text);    
  tft.println("coded by dEskoG");
  tft.setCursor(20, 50);
  tft.println("rduinoscope.co.nf");
  tft.setCursor(0, 100);
  tft.setTextColor(d_text);    
  tft.setTextSize(1);

  // EMPIRE MARCH   :)
  SoundOn(note_f, 48);
  delay(100);
  SoundOn(note_f, 48);
  delay(100);
  SoundOn(note_f, 48);
  delay(100);
  SoundOn(note_cb, 32);
  delay(140);
  SoundOn(note_gb, 8);
  delay(50);
  SoundOn(note_f, 48);
  delay(100);
  SoundOn(note_cb, 32);
  delay(140);
  SoundOn(note_gb, 8);
  delay(50);
  SoundOn(note_f, 48);

  // Indiana Jones :)
  // EFGC DEF GABF AABCDE 

  
  // see if the card is present and can be initialized:
  char in_char;
  String items="";
  int j=0;
  int k=0;
  MESS_PAGER = 0;
  TREAS_PAGER = 0;
  STARS_PAGER = 0;

  if (!SD.begin(sd_cs)) {
    tft.println("ERROR: Card failed, or not present");
    // don't do anything more:
    while(1);
  }
  tft.println("...card initialized");
  delay(400);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("messier.csv");

  // if the file is available, write to it:
  if (dataFile) {
    tft.println("...loading data from MESSIER.CSV");
    delay(700);
    while (dataFile.available()) {
        in_char=dataFile.read();
        items += in_char;
        k=k+1;
        if(in_char == '\n'){
          Messier_Array[j] = items;
          j=j+1;
//          Serial.print(items);
          items="";
        }
    }
    tft.println("...loading Messier COMPLETED");
    delay(700);
  
  }else {
    tft.println("ERROR opening: messier.csv");
  }
  
  dataFile.close();  
  items="";
  j=0;
  k=0;
  dataFile = SD.open("treasure.csv");


  // if the file is available, write to it:
  if (dataFile) {
    tft.println("...loading data from TREASURE.CSV");
    delay(700);
    while (dataFile.available()) {
        in_char=dataFile.read();
        items += in_char;
        k=k+1;
        if(in_char == '\n'){
          Treasure_Array[j] = items;
          j=j+1;
//          Serial.print(items);
          items="";
        }
    }
    tft.println("...loading Treasures COMPLETED");
    delay(700);
  
  }else {
    tft.println("ERROR opening: treasure.csv");
  }
  dataFile.close();
  last_button=0;
  LOAD_SELECTOR = 0;

  tft.println(".................................");
  tft.println("...initializing BlueTooth");
  delay(700);
  tft.println("...initializing GPS");

  delay(1000);
  CURRENT_SCREEN = 0;
  drawGPSScreen();

}

void loop(void) {
  uint8_t flag;
  int tx, ty;
  
  p=touch.getpos(&flag);
  tx=(p.x- 310)/14;
  ty=(p.y- 150)/9;
  
  calculateLST_HA();
  cosiderSlewTo();
  
  if (IS_OBJ_FOUND == true){    // Only doing this to prevent Interruptions when Motor SLEWs to point...
      if (IS_BT_MODE_ON == true){
          BT_COMMAND_STR = "";
          if (Serial3.available()){
              BT_COMMAND_STR = Serial3.readString();
              considerBTCommands();
          }
      }
      xPosition = analogRead(xPin);
      yPosition = analogRead(yPin);
      if ((xPosition < 500) || (xPosition > 800) || (yPosition < 500) || (yPosition > 800)){
        IS_MANUAL_MOVE = true;
        consider_Manual_Move(xPosition, yPosition);
        // Serial.println("mm!");
      }else{
        IS_MANUAL_MOVE = false;
      }
      considerTouchInput(tx, ty);
      considerTimeUpdates();
      considerDayNightMode();
      considerTempUpdates();
      // I need to make sure the Drives are not moved to track the stars,
      // if Object is below horizon ALT < 0 - Stop tracking.
      if ((ALT <= 0)&(IS_TRACKING==true)&(IS_IN_OPERATION == true)){
        IS_TRACKING = false;
        Timer3.stop();
        drawMainScreen();
      }
  }
}

void removeTime_addXX(){
  if (dateEntryPos == 0){
    tft.fillRect(30,55,180,22,BLACK);
    tft.fillRect(80,100,90,22, BLACK);
    tft.setTextColor(btn_l_text);
    tft.setCursor(30, 55);
    tft.print("XX/XX/XXXX");
    tft.setCursor(80, 100);
    tft.print("XX:XX");
  }
}

void considerTempUpdates(){
  if (CURRENT_SCREEN == 4 & (millis()-Tupdate_time) > 10000){
       float tHum = dht.readHumidity();
       float tTemp = dht.readTemperature();
       if (isnan(tHum) || isnan(tTemp)) {
          return;
       }else{
          _temp = tTemp;
          _humid = tHum;
       }
       tft.setTextSize(2);
       tft.setTextColor(title_texts);
       if (_temp > -75 & _temp < 75 & _humid < 100 & _humid > 0){
         tft.fillRect(190,31,25,35, title_bg);
         tft.setCursor(190, 31);
         tft.print(_temp,0);
         tft.setCursor(190, 47);
         tft.print(_humid,0);
       }
       Tupdate_time = millis();
  }
}

void considerTimeUpdates(){   // UPDATEs time on Screen1 & Screen4 -  Clock Screen and Main Screen
   int changes=0;
   for (int y=0; y<12; y++){
       if (w_DateTime[y]!=0){
           changes=1;
       }
   }
  int mer_flp_sound = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP + MIN_SOUND_BEFORE_FLIP)/60);
  if((millis()- update_time) > 10000 & mer_flp_sound >= 24){
      SoundOn(3830,8);
  }
  if (CURRENT_SCREEN == 4 & (millis()-update_time) > 10000){
      tft.setTextSize(3);
      tft.setTextColor(title_texts);
      if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/')){
            tft.fillRect(6,6,180,22, title_bg);
            tft.setCursor(6,6);
            tft.print(String(rtc.getDateStr()).substring(0,2));
            tft.print(" ");
            tft.print(rtc.getMonthStr(FORMAT_SHORT));
            tft.print(" ");
            tft.print(String(rtc.getDateStr()).substring(6));
      }
      tft.fillRect(65,31,100,17, title_bg);
      tft.setTextSize(2);
      tft.setCursor(65, 31);
      tft.print(String(rtc.getTimeStr()).substring(0,5));       
      tft.fillRect(65,48,100,17, title_bg);
      tft.setCursor(65, 47);
      if ((int)LST < 10){
        tft.print("0");
        tft.print((int)LST);
      }else{
        tft.print((int)LST);
      }
      tft.print(":");
      if ((LST - (int)LST)*60 < 10){
        tft.print("0");
        tft.print((LST - (int)LST)*60, 1);
      }else{
        tft.print((LST - (int)LST)*60, 1);
      }
      if ((OBJECT_NAME != "") & (OBJECT_NAME != "CP") & (IS_BT_MODE_ON == false)){
          tft.setCursor(130,259);
          tft.setTextColor(l_text);
          tft.fillRect(127,255,112,20, BLACK);
          tft.print(HAHour,0);
          tft.print("h");
          tft.print(HAMin,1);
          tft.print("m");
      }
      if ((IS_BT_MODE_ON == true)&(OBJECT_DESCR == "Pushed via BlueTooth")){
          tft.setTextSize(1);
          tft.setCursor(5,256);
          tft.setTextColor(l_text);
          tft.fillRect(5,255,230,22, BLACK);
          tft.print("HA: ");
          tft.print(HAHour,0);
          tft.print("h");
          tft.print(HAMin,1);
          tft.print("m    | HS:");
          tft.println(SLEW_RA_microsteps);
          tft.print("               | CS:");
          tft.print(RA_microSteps);
      }   
          
      update_time = millis();
  }else if (CURRENT_SCREEN == 5 & (millis()-update_time) > 2000){
      tft.setTextSize(2);
      
      // Calculate & Draw coordinates in boxes
      // DECLINATION
      float tmp_dec = (float(DEC_90) - float(abs(DEC_microSteps)))/float(DEC_D_CONST);
      float temp_dec_m = (tmp_dec - int(tmp_dec))*60;
      if (tmp_dec < 0){
        tmp_dec -= 1;
        temp_dec_m += 60;
      }
      tft.fillRect(52,117,165,20, btn_d_border);
      tft.setCursor(65, 120);
      tft.print(tmp_dec, 0);
      tft.print("d ");
      tft.print(abs(temp_dec_m), 2);
      tft.print("m");

      // HOUR ANGLE
      float tmp_ha = float(RA_microSteps)/float(HA_H_CONST); 
      if (DEC_microSteps > 0){
          tmp_ha +=180;
      }
      tmp_ha /= 15;
      float tmp_ha_m = (tmp_ha - int(tmp_ha))*60;
      tft.fillRect(52,57,165,20, btn_d_border);
      tft.setCursor(65, 60);
      tft.print(tmp_ha, 0);
      tft.print("h ");
      tft.print(tmp_ha_m, 2);
      tft.print("m");

      // RIGHT ASC.
      tft.fillRect(52,87,165,20, btn_d_border);
      float tmp_ra = LST - tmp_ha; 
      if (LST < tmp_ha){
        tmp_ra = 24 - abs(tmp_ra);
      }
      tft.setCursor(65, 90);
      tft.print(abs(tmp_ra), 0);
      tft.print("h ");
      tft.print(abs((tmp_ra - int(tmp_ra))*60), 2);
      tft.print("m");

     update_time = millis();  

  }else if (CURRENT_SCREEN == 1 & (millis()-update_time) > 10000 & changes == 0){
      tft.setCursor(80, 100);
      tft.setTextColor(l_text);
      tft.fillRect(80,100,180,22, BLACK);
      tft.print(rtc.getTimeStr(FORMAT_SHORT));
      if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/')){
          tft.fillRect(30,55,180,22, BLACK);
          tft.setCursor(30, 55);
          tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
      }
      update_time = millis();      
  }else if (CURRENT_SCREEN == 0 & (millis()-update_time) > 5000){
        tft.fillRect(10,195,230,95, BLACK);
        tft.setTextColor(btn_l_text);
        tft.setTextSize(3);
        tft.setCursor(10, 200);
        tft.print("LAT: ");
        tft.print(gps.location.lat(),4);
        tft.setCursor(10, 230);
        tft.print("LNG: ");
        tft.print(gps.location.lng(),4);
        tft.setCursor(60, 260);
        tft.setTextSize(1);
        tft.print("Satelites: ");
        tft.print(gps.satellites.value());
        tft.setCursor(60, 270);
        tft.print("GMT: ");
        tft.print(gps.time.hour());
        tft.print(":");
        tft.print(gps.time.minute());
        tft.setCursor(60, 280);
        tft.print("Altitude: ");
        tft.print(gps.altitude.meters());
        if (gps.satellites.value()==0){
          smartDelay(1000);
        }else{
          GPS_iterrations += 1;
          smartDelay(1000);
        }

        if ((GPS_iterrations > 2) & (gps.location.lng() !=0)){
          OBSERVATION_LONGITUDE = gps.location.lng();
          OBSERVATION_LATTITUDE = gps.location.lat();
          OBSERVATION_ALTITUDE = gps.altitude.meters();
          // Set the earth rotation direction depending on the Hemisphere...
          // HIGH and LOW are substituted 
          if (OBSERVATION_LONGITUDE > 0){
            STP_FWD = HIGH;
            STP_BACK = LOW;
          }else{
            STP_FWD = LOW;
            STP_BACK = HIGH;
          }
          SoundOn(note_C,32);
          CURRENT_SCREEN = 1;
          // Serial2.end();
          drawClockScreen();
        }
        update_time = millis(); 
  }else if ((CURRENT_SCREEN == 13) & (IS_OBJ_FOUND == true) & ((millis()-update_time) > 1000)){
        tft.setTextColor(btn_l_text);
        tft.setTextSize(1);
        tft.fillRect(140,245,100,25, BLACK);
        tft.setCursor(145, 248);
        tft.print(SLEW_RA_microsteps);
        tft.setCursor(145, 256);
        tft.print(SLEW_DEC_microsteps);
        tft.fillRect(130,265,110,80, BLACK);
        tft.setTextSize(2);
        float angle_RA = float(SLEW_RA_microsteps)/float(RA_microSteps);
        float angle_DEC = float(SLEW_DEC_microsteps)/float(DEC_microSteps);
        tft.setCursor(132, 270);
        tft.print(SLEW_RA_microsteps - RA_microSteps);
        tft.setCursor(132, 287);
        tft.print(1-angle_RA,5);
        tft.setCursor(132, 304);
        tft.print(SLEW_DEC_microsteps - DEC_microSteps);
        tft.setCursor(132, 321);
        tft.print(1-angle_DEC,5);
        update_time = millis();
  }
    
}

void calculateLST_HA(){
//  HA = LST - RA 
    String Date_q = String(rtc.getDateStr());
    String Time_q = String(rtc.getTimeStr());
    int D = Date_q.substring(0,2).toInt();
    int M = Date_q.substring(3,5).toInt();
    int Y = Date_q.substring(6).toInt();
    int S = Time_q.substring(6).toInt();
    float H; // hours
    if (Summer_Time == 1){
      H = Time_q.substring(0,2).toInt()-1;
    }else{
      H = Time_q.substring(0,2).toInt();
    }
    int MN = Time_q.substring(3,5).toInt();
    if (M < 3) {
       M = M + 12; 
       Y = Y - 1;
    }
    
    float HH = H + ((float)MN/60.00) + ((float)S/3600.00);
    float AA= (int)(365.25*(Y+4716));
    float BB= (int)(30.6001*(M+1)); 
    double CurrentJDN = AA + BB + D - 1537.5 + (HH-TIME_ZONE)/24;
    float current_day = CurrentJDN - 2451543.5;

    //calculate terms required for LST calcuation and calculate GMST using an approximation
    double MJD = CurrentJDN - 2400000.5;		
    int MJD0 = (int)MJD;
    float ut = (MJD - MJD0)*24.0;
    double t_eph  = (MJD0-51544.5)/36525.0;			
    double GMST = 6.697374558 + 1.0027379093*ut + (8640184.812866 + (0.093104 - 0.0000062*t_eph)*t_eph)*t_eph/3600.0;		

    LST = GMST + OBSERVATION_LONGITUDE/15.0;

    //reduce it to 24 format
    int LSTint = (int)LST;
    LSTint/=24;
    LST = LST - (double) LSTint * 24;

// Now I'll use the global Variables OBJECT_RA_H and OBJECT_RA_M  To calculate the Hour angle of the selected object.
    
    double d_OBJECT_RA_M = ((int)OBJECT_RA_M);
    double dec_RA = d_OBJECT_RA_M/60 + ((OBJECT_RA_M - d_OBJECT_RA_M)/10) + OBJECT_RA_H;
    double HA_decimal = LST - dec_RA;
    HAHour = int(HA_decimal);
    HAMin = (HA_decimal - HAHour)*60; 
    
    if (HAMin < 0){
      HAHour -= 1;
      HAMin +=60;
    }
    if (HAHour < 0){
      HAHour +=24;
    }
    
    // Convert degrees into Decimal Radians
    double rDEC = ((OBJECT_DEC_D + (OBJECT_DEC_M/60))*3.141593)/180;
    double rHA =  (HA_decimal*47.123889)/180;     // 47.12.. = 15 * 3.1415...  (to convert to Deg. and * Pi) :)
    double rLAT = (OBSERVATION_LATTITUDE*3.141593)/180;

IS_OBJ_VISIBLE = true;

if (IS_IN_OPERATION == true){
    ALT = sin(rDEC)*sin(rLAT);
    ALT += cos(rDEC)*cos(rLAT)*cos(rHA);
    
    if (ALT < 0){
      IS_OBJ_VISIBLE = false;
      if ((IS_BT_MODE_ON == true)&(IS_OBJ_FOUND == false)){
         Serial3.println("Object is out of sight! Telescope not moved.");
      }
      IS_OBJ_FOUND = true;
      IS_OBJECT_RA_FOUND = true;
      IS_OBJECT_DEC_FOUND = true;
      Slew_RA_timer = 0;
      RA_finish_last = 0;
    }else{
      IS_OBJ_VISIBLE = true;
    }
}
    // Take care of the Meridian Flip coordinates
    mer_flp = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP)/60);
    float old_HAMin = HAMin;
    float old_HAHour = HAHour;
  if (mer_flp >= 24){
        HAMin = HAMin - 60;
        HAHour = 0;
        if (MERIDIAN_FLIP_DO == false){
            IS_TRACKING = false;
            Timer3.stop();
            OnScreenMsg(1);
              SoundOn(note_C,32);
              delay(200);
              SoundOn(note_C,32);
              delay(200);
              SoundOn(note_C,32);
              delay(1000);            
            IS_OBJ_FOUND = false;
            IS_OBJECT_RA_FOUND = false;
            IS_OBJECT_DEC_FOUND = false;
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            MERIDIAN_FLIP_DO = true;
            drawMainScreen();
        }else{
            if ((old_HAHour == HAHour) & (old_HAMin == HAMin)){   // Meridian Flip is done so the code above will not execute
                MERIDIAN_FLIP_DO = false;
            }
        }
        //DEC is set as part of the SlewTo function
    }
}

void selectOBJECT_M(int index_, int objects) {
   OBJECT_Index = index_;
   if (objects == 0){
     // I've selected a Messier Object
      int i1 = Messier_Array[index_].indexOf(';');
      int i2 = Messier_Array[index_].indexOf(';',i1+1);
      int i3 = Messier_Array[index_].indexOf(';',i2+1);
      int i4 = Messier_Array[index_].indexOf(';',i3+1);
      int i5 = Messier_Array[index_].indexOf(';',i4+1);
      int i6 = Messier_Array[index_].indexOf(';',i5+1);
      int i7 = Messier_Array[index_].indexOf(';',i6+1);
      OBJECT_NAME = Messier_Array[index_].substring(0,i1);
      OBJECT_DESCR = Messier_Array[index_].substring(i7+1,Messier_Array[index_].length()-1);
      String OBJ_RA = Messier_Array[index_].substring(i1,i2);
      OBJECT_RA_H = OBJ_RA.substring(1,OBJ_RA.indexOf('h')).toFloat();
      OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h')+1,OBJ_RA.length()-1).toFloat();
      String OBJ_DEC = Messier_Array[index_].substring(i2,i3);
      String sign = OBJ_DEC.substring(1,2);
      OBJECT_DEC_D = OBJ_DEC.substring(2,OBJ_DEC.indexOf('°')).toFloat();
      if (sign == "-"){ 
          OBJECT_DEC_D *= (-1);
      }
      OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°')+1,OBJ_DEC.length()-1).toFloat();
      OBJECT_DETAILS = OBJECT_NAME + " is a ";
      OBJECT_DETAILS += Messier_Array[index_].substring(i4+1,i5) + " in constelation ";
      OBJECT_DETAILS += Messier_Array[index_].substring(i3+1,i4) + ", with visible magnitude of ";
      OBJECT_DETAILS += Messier_Array[index_].substring(i5+1,i6) + " and size of ";
      OBJECT_DETAILS += Messier_Array[index_].substring(i6+1,i7);
   }else if (objects == 1){
      // I've selected a treasure Object
      int i1 = Treasure_Array[index_].indexOf(';');
      int i2 = Treasure_Array[index_].indexOf(';',i1+1);
      int i3 = Treasure_Array[index_].indexOf(';',i2+1);
      int i4 = Treasure_Array[index_].indexOf(';',i3+1);
      int i5 = Treasure_Array[index_].indexOf(';',i4+1);
      int i6 = Treasure_Array[index_].indexOf(';',i5+1);
      int i7 = Treasure_Array[index_].indexOf(';',i6+1);
      OBJECT_NAME = Treasure_Array[index_].substring(0,i1);
      OBJECT_DESCR = Treasure_Array[index_].substring(i7+1,Treasure_Array[index_].length()-1);
      String OBJ_RA = Treasure_Array[index_].substring(i1,i2);
      OBJECT_RA_H = OBJ_RA.substring(1,OBJ_RA.indexOf('h')).toFloat();
      OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h')+1,OBJ_RA.length()-1).toFloat();
      String OBJ_DEC = Treasure_Array[index_].substring(i2,i3);
      String sign = OBJ_DEC.substring(1,2);
      OBJECT_DEC_D = OBJ_DEC.substring(2,OBJ_DEC.indexOf('°')).toFloat();
      if (sign == "-"){ 
          OBJECT_DEC_D *= (-1);
      }
      OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°')+1,OBJ_DEC.length()-1).toFloat();
      OBJECT_DETAILS = OBJECT_NAME + " is a ";
      OBJECT_DETAILS += Treasure_Array[index_].substring(i4+1,i5) + " in constelation ";
      OBJECT_DETAILS += Treasure_Array[index_].substring(i3+1,i4) + ", with visible magnitude of ";
      OBJECT_DETAILS += Treasure_Array[index_].substring(i5+1,i6) + " and size of ";
      OBJECT_DETAILS += Treasure_Array[index_].substring(i6+1,i7);
   }else if (objects == 2){
    // I'm selecting a STAR for Synchronization
      int i1 = Stars[index_].indexOf(';');
      int i2 = Stars[index_].indexOf(';',i1+1);
      int i3 = Stars[index_].indexOf(';',i2+1);
      OBJECT_NAME = Stars[index_].substring(i1+1,i2)+" from "+Stars[index_].substring(0,i1);
      String OBJ_RA = Stars[index_].substring(i2,i3);
      OBJECT_RA_H = OBJ_RA.substring(1,OBJ_RA.indexOf('h')).toFloat();
      OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h')+1,OBJ_RA.length()-1).toFloat();
      String OBJ_DEC = Stars[index_].substring(i3,Stars[index_].length());
      String sign = OBJ_DEC.substring(1,2);
      OBJECT_DEC_D = OBJ_DEC.substring(2,OBJ_DEC.indexOf('°')).toFloat();
      if (sign == "-"){ 
          OBJECT_DEC_D *= (-1);
      }
      OBJECT_DEC_M = OBJ_DEC.substring(OBJ_DEC.indexOf('°')+1,OBJ_DEC.length()-1).toFloat();
   }
}

void considerTouchInput(int lx, int ly){
  if (lx > 0 & ly > 0 ){
    if (CURRENT_SCREEN == 0){ 
      if (lx > 46 & lx < 192 & ly > 317 & ly < 371){
        last_button = 1;
        tft.fillRect(45,316,148,58, btn_l_border);
      }
    }else if (CURRENT_SCREEN == 1){
       if (lx > 160 & lx < 230 & ly > 336 & ly < 393){
         // BTN OK pressed
           tft.fillRect(160,336,72,57, btn_l_border);
           int changes=0;
           for (int y=0; y<12; y++){
             if (w_DateTime[y]!=0){
               changes=1;
             }
           }
           if (changes == 1){
             // Do the magic as the date and time has been updated... Update the RTC accordingly
             int hh = (w_DateTime[8]*10) + w_DateTime[9];
             int mm = (w_DateTime[10]*10) + w_DateTime[11];
             int dd = (w_DateTime[0]*10) + w_DateTime[1];
             int mo = (w_DateTime[2]*10) + w_DateTime[3];
             int yy = (w_DateTime[4]*1000) + (w_DateTime[5]*100) + (w_DateTime[6]*10) + w_DateTime[7];
             rtc.setTime(hh,mm,00);
             rtc.setDate(dd,mo,yy);
           }
           Start_date = String(rtc.getDateStr()).substring(0,2)+" "+rtc.getMonthStr(FORMAT_SHORT)+" "+String(rtc.getDateStr()).substring(6);
           old_d = rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/');
           START_TIME = rtc.getTimeStr(FORMAT_SHORT);
           CURRENT_SCREEN = 4;
           _Stemp = dht.readTemperature();;
           _Shumid = dht.readHumidity();
           drawStarSyncScreen();
           //drawMainScreen();
       }else if (lx > 11 & lx < 83 & ly > 156 & ly < 213){
         // BTN 1 pressed
          tft.drawRect(11,156,72,57, btn_l_border);
          last_button = 1;
          removeTime_addXX();
       }else if (lx > 86 & lx < 158 & ly > 156 & ly < 213){
         // BTN 2 pressed
          tft.drawRect(86,156,72,57, btn_l_border);
          last_button = 2;
          removeTime_addXX();
       }else if (lx > 161 & lx < 233 & ly > 156 & ly < 213){
         // BTN 3 pressed
          tft.drawRect(161,156,72,57, btn_l_border);
          last_button = 3;
          removeTime_addXX();
       }else if (lx > 11 & lx < 83 & ly > 216 & ly < 273){
         // BTN 4 pressed
          tft.drawRect(11,216,72,57, btn_l_border);
          last_button = 4;
          removeTime_addXX();
       }else if (lx > 86 & lx < 158 & ly > 216 & ly < 273){
         // BTN 5 pressed
          tft.drawRect(86,216,72,57, btn_l_border);
          last_button = 5;
          removeTime_addXX();
       }else if (lx > 161 & lx < 233 & ly > 216 & ly < 273){
         // BTN 6 pressed
          tft.drawRect(161,216,72,57, btn_l_border);
          last_button = 6;
          removeTime_addXX();
       }else if (lx > 11 & lx < 83 & ly > 276 & ly < 333){
         // BTN 7 pressed
          tft.drawRect(11,276,72,57, btn_l_border);
          last_button = 7;
          removeTime_addXX();
       }else if (lx > 86 & lx < 158 & ly > 276 & ly < 333){
         // BTN 8 pressed
          tft.drawRect(86,276,72,57, btn_l_border);
          last_button = 8;
          removeTime_addXX();
       }else if (lx > 161 & lx < 233 & ly > 276 & ly < 333){
         // BTN 9 pressed
          tft.drawRect(161,276,72,57, btn_l_border);
          last_button = 9;
          removeTime_addXX();
       }else if (lx > 86 & lx < 158 & ly > 336 & ly < 393){
         // BTN 0 pressed
          tft.drawRect(86,336,72,57, btn_l_border);
          last_button = 10;
          removeTime_addXX();
       } else if (lx > 11 & lx < 83 & ly > 336 & ly < 393){
       // BTN SummerTime pressed
         last_button = 22;
      }
    }else if (CURRENT_SCREEN == 4){
      if (lx > 0 & lx < 55 & ly > 0 & ly < 55){
       // Load the GPS screen to capture new data & correct time if needed on the next screen (Time Screen)
         tft.drawRect(0,0,55,55, btn_l_border);
         CURRENT_SCREEN =0;
         GPS_iterrations = 0;
         IS_IN_OPERATION = false;
         drawGPSScreen();
      }
      if (lx > 2 & lx < 78 & ly > 282 & ly < 335 & IS_BT_MODE_ON == false){
       // BTN 1 pressed
         tft.fillRect(2,281,76,55, btn_l_border);
         last_button = 1;
      } else if (lx > 2 & lx < 78 & ly > 341 & ly < 400 & IS_BT_MODE_ON == false){
       // BTN 4 pressed
         tft.fillRect(2,340,76,55, btn_l_border);
         last_button = 4;
      } else if (lx > 83 & lx < 148 & ly > 280 & ly < 335 & IS_BT_MODE_ON == false){
       // BTN 2 pressed
         tft.fillRect(82,281,76,55, btn_l_border);
         last_button = 2;
      } else if (lx > 83 & lx < 148 & ly > 341 & ly < 400 & IS_BT_MODE_ON == false){
       // BTN 5 pressed
         tft.fillRect(82,340,76,55, btn_l_border);
         last_button = 5;
      } else if (lx > 163 & lx < 239 & ly > 282 & ly < 335){
       // BTN 3 pressed
         tft.fillRect(162,281,76,55, btn_l_border);
         last_button = 3;
      } else if (lx > 163 & lx < 239 & ly > 341 & ly < 400){
       // BTN 6 pressed
         tft.fillRect(162,340,76,55, btn_l_border);
         last_button = 6;
      }
    }else if (CURRENT_SCREEN == 5){
       if (lx > 181 & lx < 238 & ly > 5 & ly < 35){
       // BTN Cancel pressed
         tft.fillRect(180,5,58,25, btn_l_border);
         drawMainScreen();
       }
    }else if (CURRENT_SCREEN == 6){
       if (lx > 181 & lx < 238 & ly > 5 & ly < 35){
       // BTN Cancel pressed
         tft.fillRect(180,5,58,25, btn_l_border);
         drawMainScreen();
       }
       if (lx > 157 & lx < 238 & ly > 360 & ly < 400){
       // BTN next> pressed  TREAS_PAGER
          if (LOAD_SELECTOR == 1){
             MESS_PAGER += 1;
             if (MESS_PAGER < 8){
                drawLoadScreen();
             }else{
               MESS_PAGER = 6;
             }
          }else {
             TREAS_PAGER += 1;
             if (TREAS_PAGER < 9){
                drawLoadScreen();
             }else{
               TREAS_PAGER = 8;
             }
          }
       }
       if (lx > 5 & lx < 85 & ly > 360 & ly < 400){
       // BTN <prev pressed
          if (LOAD_SELECTOR == 1){
             MESS_PAGER -= 1;
             if (MESS_PAGER >= 0){
                drawLoadScreen();
             }else{
                MESS_PAGER = 0;
             }
          }else{
             TREAS_PAGER -= 1;
             if (TREAS_PAGER >= 0){
                drawLoadScreen();
             }else{
                TREAS_PAGER = 0;
             }
          }
       }
       if (lx > 1 & lx < 117 & ly > 40 & ly < 70){
       // BTN Messier pressed
           LOAD_SELECTOR = 1;
           drawLoadScreen();
       }
       if (lx > 120 & lx < 240 & ly > 40 & ly < 70){
       // BTN File pressed
           LOAD_SELECTOR = 2;
           drawLoadScreen();
       }       
       if (LOAD_SELECTOR == 1){       
         // I'm in MESSIER selector and need to check which Messier object is pressed
         for (int i=0; i<5; i++){
            for (int j=0; j<3; j++){
              if (lx > ((j*75)+10) & lx < ((j*75)+79) & ly > ((i*50)+86) & ly < ((i*50)+129)){
                 // found button pressed.... now I need to get his ID and link to the ARRAY;
                int zz = (MESS_PAGER*15) + (i*3) + j;
                if (Messier_Array[zz] != ""){
                  selectOBJECT_M(zz,0);
                  calculateLST_HA();
                  if (ALT > 0){
                     OnScreenMsg(1);
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(1000);
                     IS_OBJ_FOUND = false;
                     IS_OBJECT_RA_FOUND = false;
                     IS_OBJECT_DEC_FOUND = false;
                     Slew_timer = millis();
                     Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                  }
                  UpdateObservedObjects();
                  MESS_PAGER == 0;
                  drawMainScreen();
                }
              }
            }
         }
       }else if (LOAD_SELECTOR == 2){       
         // I'm in TREASURES selector and need to check which Treasure object is pressed
         for (int i=0; i<5; i++){
            for (int j=0; j<3; j++){
              if (lx > ((j*75)+10) & lx < ((j*75)+79) & ly > ((i*50)+86) & ly < ((i*50)+129)){
                 // found button pressed.... now I need to get his ID and link to the ARRAY;
                int zz = (TREAS_PAGER*15) + (i*3) + j;
                if (Treasure_Array[zz] != ""){
                  selectOBJECT_M(zz,1);
                  calculateLST_HA();
                  if (ALT > 0){
                     OnScreenMsg(1);
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(1000);
                     IS_OBJ_FOUND = false;
                     IS_OBJECT_RA_FOUND = false;
                     IS_OBJECT_DEC_FOUND = false;
                     Slew_timer = millis();
                     Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                  }
                  UpdateObservedObjects();
                  TREAS_PAGER == 0;
                  drawMainScreen();
                }
              }
            }
         }
       }
     }else if (CURRENT_SCREEN == 10){
       if (lx > 181 & lx < 238 & ly > 5 & ly < 35){
       // BTN Cancel pressed
         tft.fillRect(180,5,58,25, btn_l_border);
         IS_IN_OPERATION = true;
         drawMainScreen();
        }
    }else if (CURRENT_SCREEN == 12){
       if (lx > 181 & lx < 238 & ly > 5 & ly < 35){
       // BTN SKIP pressed
         tft.fillRect(180,5,58,25, btn_l_border);
         IS_IN_OPERATION = true;
         drawMainScreen();
       }
       if (lx > 157 & lx < 238 & ly > 360 & ly < 400){
       // BTN next> pressed 
             STARS_PAGER += 1;
             if (STARS_PAGER < 8){
                drawStarSyncScreen();
             }else{
               STARS_PAGER = 6;
             }
       }
       if (lx > 5 & lx < 85 & ly > 360 & ly < 400){
       // BTN <prev pressed
             STARS_PAGER -= 1;
             if (STARS_PAGER >= 0){
                drawStarSyncScreen();
             }else{
                STARS_PAGER = 0;
             }
        }           
         // I'm in STARS selector and need to check which Star object is pressed
         for (int i=0; i<5; i++){
            for (int j=0; j<3; j++){
              if (lx > ((j*75)+10) & lx < ((j*75)+79) & ly > ((i*50)+86) & ly < ((i*50)+129)){
                 // found button pressed.... now I need to get his ID and link to the ARRAY;
                int zz = (STARS_PAGER*15) + (i*3) + j;
                if (Stars[zz] != ""){
                  selectOBJECT_M(zz,2);
                  calculateLST_HA();
//                  if (ALT > 0){
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(200);
                     SoundOn(note_C,32);
                     delay(200);
                     IS_OBJ_FOUND = false;
                     IS_OBJECT_RA_FOUND = false;
                     IS_OBJECT_DEC_FOUND = false;
                     Slew_timer = millis();
                     Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                     STARS_PAGER == 0;
                     SELECTED_STAR = zz;
                     drawConstelationScreen(zz);
//                  }else{
//                    SoundOn(note_X,128);
//                    drawStarSyncScreen();
//                  }
                }
              }
            }
         }
    }else if (CURRENT_SCREEN==13){
        if (lx > 5 & lx < 105 & ly > 360 & ly < 400){
          // BTN "<Repeat" pressed
          drawStarSyncScreen();
        }
        if (lx > 128 & lx < 240 & ly > 360 & ly < 400){
          // BTN "ALIGN!" pressed
          DELTA_RA_ADJUST = float(SLEW_RA_microsteps)/float(RA_microSteps);
          DELTA_DEC_ADJUST = float(SLEW_DEC_microsteps)/float(DEC_microSteps);
          IS_IN_OPERATION = true;
          OBJECT_NAME = "";
          OBJECT_RA_H = 0;
          OBJECT_RA_M = 0;
          OBJECT_DEC_D = 0;
          OBJECT_DEC_M = 0;
          drawMainScreen();
        }
    }
  }else{
    if (CURRENT_SCREEN == 0){ 
      if (last_button == 1){
          last_button = 0;
          // Set the earth rotation direction depending on the Hemisphere...
          // HIGH and LOW are substituted 
          if (OBSERVATION_LATTITUDE > 0){
            STP_FWD = HIGH;
            STP_BACK = LOW;
          }else{
            STP_FWD = LOW;
            STP_BACK = HIGH;
          }
          CURRENT_SCREEN = 1;
          // Serial2.end();
          drawClockScreen();
      }
    }else if (CURRENT_SCREEN == 1){
        if (last_button == 1){
         last_button = 0;
         tft.drawRect(11,156,72,57, BLACK);
         TimerUpdateDraw(1);
        }
        if (last_button == 2){
         last_button = 0;
         tft.drawRect(86,156,72,57, BLACK);
         TimerUpdateDraw(2);
        }
        if (last_button == 3){
         last_button = 0;
         tft.drawRect(161,156,72,57, BLACK);
         TimerUpdateDraw(3);
        }
        if (last_button == 4){
         last_button = 0;
         tft.drawRect(11,216,72,57, BLACK);
         TimerUpdateDraw(4);
        }
        if (last_button == 5){
         last_button = 0;
         tft.drawRect(86,216,72,57, BLACK);
         TimerUpdateDraw(5);
        }
        if (last_button == 6){
         last_button = 0;
         tft.drawRect(161,216,72,57, BLACK);
         TimerUpdateDraw(6);
        }
        if (last_button == 7){
         last_button = 0;
         tft.drawRect(11,276,72,57, BLACK);
         TimerUpdateDraw(7);
        }
        if (last_button == 8){
         last_button = 0;
         tft.drawRect(86,276,72,57, BLACK);
         TimerUpdateDraw(8);
        }
        if (last_button == 9){
         last_button = 0;
         tft.drawRect(161,276,72,57, BLACK);
         TimerUpdateDraw(9);
        }
        if (last_button == 10){
         last_button = 0;
         tft.drawRect(86,336,72,57, BLACK);
         TimerUpdateDraw(0);
        }
        if (last_button == 22){
            if (Summer_Time == 1){
                Summer_Time = 0;
                tft.drawRect(11,336,71,56, btn_d_border); 
                tft.fillRect(12,337,69,54, BLACK);
                tft.setTextColor(btn_d_text);
            }else{
                Summer_Time = 1;
                tft.drawRect(11,336,71,56, btn_l_border);
                tft.fillRect(12,337,69,54, btn_d_border);
                tft.setTextColor(btn_l_text);
            }
            tft.setTextSize(1);
            tft.setCursor(32, 352);
            tft.println("SUMMER");
            tft.setCursor(37, 365);
            tft.println("TIME");
            tft.setTextColor(btn_l_text);
            tft.setTextSize(3);
            last_button = 0;
        }
    }else if (CURRENT_SCREEN == 4){
        tft.setTextColor(btn_l_text);
        if (last_button == 1 & IS_BT_MODE_ON == false){
         last_button = 0;
         tft.fillRect(2,281,76,55, BLACK);
         tft.setCursor(15, 300);
         tft.println("LOAD");
         drawLoadScreen();
        }
        if (last_button == 4 & IS_BT_MODE_ON == false){
         last_button = 0;
         tft.fillRect(2,340,76,55, BLACK);
         tft.setCursor(10, 360);
         tft.println("INPUT");
         if (IS_TRACKING == true){
//            OnScreenMsg(2);
            SoundOn(note_C,32);
//            delay(2000);
         }
         drawCoordinatesScreen();
        }
        if (last_button == 2 & IS_BT_MODE_ON == false){
         last_button = 0;
         tft.fillRect(82,281,76,55, BLACK);
         tft.setCursor(97, 292);
         tft.println("GoTo");
         tft.setCursor(97, 310);
         tft.println("Home");
         OBJECT_NAME = "CP";
         OBJECT_DESCR = "Celestial pole";
         OBJECT_RA_H = 12;
         OBJECT_RA_M = 0;       
         OBJECT_DEC_D = 90;
         OBJECT_DEC_M = 0;
         IS_OBJ_FOUND = false;
         IS_OBJECT_RA_FOUND = false;
         IS_OBJECT_DEC_FOUND = false;
         OBJECT_DETAILS="The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere";
         IS_TRACKING = false;
        }
        if (last_button == 5 & IS_BT_MODE_ON == false){
         last_button = 0;
         tft.fillRect(82,340,76,55, BLACK);
         tft.setCursor(103, 360);
         tft.println("...");
         drawSTATScreen();
        }
        if (last_button == 3){
         last_button = 0;
         if (IS_TRACKING == false){
           IS_TRACKING = true;
           tft.fillRect(162,281,76,55, btn_d_border);
           tft.setTextColor(btn_l_text);
           // Setting low speed of Stepper -> 16 microsteps;
            setmStepsMode("R",MICROSteps);
           // Starting Interrupt procedure for tracking
           Timer3.start(Clock_Motor); 
         }else{
           IS_TRACKING = false;
           tft.fillRect(162,281,76,55, BLACK);
           tft.setTextColor(btn_l_text);
           // Setting high speed of Stepper -> 1 steps;
            setmStepsMode("R",1);
           // Starting Interrupt procedure for tracking
           Timer3.stop(); // 
         }       
        tft.setCursor(172, 300);
        tft.println("TRACK");
        }
        if (last_button == 6){    // BlueTooth Mode
          if (IS_BT_MODE_ON == true){
            IS_BT_MODE_ON = false;
          }else{
            IS_BT_MODE_ON = true;
              // Initialize Bluetooth communication on PINs: 15 (RX) and 14 (TX) 
          }
          last_button = 0;
          drawMainScreen();
      }
    }
  }
}

void drawMainScreen(){
  tft.fillScreen(BLACK);
  CURRENT_SCREEN = 4;
  w_DateTime[0] = 0;
  calculateLST_HA();
  update_time = millis();
  tft.fillScreen(BLACK);
  tft.fillRect(1, 1, 239, 65, title_bg);
  tft.drawLine(0, 67, 240, 67, title_bg);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.setCursor(6, 6);
  tft.print(String(rtc.getDateStr()).substring(0,2));
  tft.print(" ");
  tft.print(rtc.getMonthStr(FORMAT_SHORT));
  tft.print(" ");
  tft.print(String(rtc.getDateStr()).substring(6));
  tft.setCursor(6, 31);
  tft.setTextSize(2);
  tft.print("Time:");
  tft.print(String(rtc.getTimeStr()).substring(0,5));
  tft.setCursor(6, 47);
  tft.print("LST :");
  if ((int)LST < 10){
    tft.print("0");
    tft.print((int)LST);
  }else{
    tft.print((int)LST);
  }
  tft.print(":");
  if ((LST - (int)LST)*60 < 10){
    tft.print("0");
    tft.print((LST - (int)LST)*60, 1);
  }else{
    tft.print((LST - (int)LST)*60, 1);
  }

  tft.setCursor(190, 31);
  tft.print(_temp,0);
  tft.setCursor(218, 31);
  tft.setTextSize(1);
  tft.print("o");
  tft.setTextSize(2);
  tft.print("C");
  tft.setCursor(190, 47);
  tft.print(_humid,0);
  tft.setCursor(222, 47);
  tft.print("%");
  tft.setTextColor(l_text);
  tft.setCursor(2, 72);
  tft.setTextSize(1);
  tft.print("LAT:");
  tft.print(OBSERVATION_LATTITUDE,4);
  tft.print(" LONG:");
  tft.print(OBSERVATION_LONGITUDE,4);
  tft.print(" ALT:");
  tft.print(OBSERVATION_ALTITUDE,0);

  tft.setTextSize(2);
  tft.setCursor(0, 100);
  tft.println("OBSERVING:");

// Data for the observed object....
if (OBJECT_NAME != ""){
  tft.setTextSize(4);
  tft.setTextColor(texts);
  tft.print(OBJECT_NAME);
  tft.setTextSize(2);
      if ((OBJECT_NAME != "CP") & (IS_BT_MODE_ON == false)){
          tft.setCursor(95,259);
          tft.println("HA:");
          tft.setCursor(130,260);
          tft.setTextColor(l_text);
          tft.print(HAHour,0);
          tft.print("h");
          tft.print(HAMin,1);
          tft.print("m");
      }
  tft.setCursor(0, 150);
  if ((OBJECT_NAME != "") & (ALT < 0)){
    tft.setTextColor(RED);
    tft.println("OBJECT not VISIBLE!");
    tft.fillRect(177,200,50,50, Maroon);
    tft.setTextSize(5);
    tft.setCursor(200, 210);
    tft.print("!");
  }else{  
    tft.println(OBJECT_DESCR);
  }
  tft.setCursor(0, 170);
  tft.setTextColor(texts);
  tft.setTextSize(1);
  tft.println(OBJECT_DETAILS);
  tft.println("");
  tft.setTextSize(2);
  tft.print("RA:");
  tft.print(OBJECT_RA_H,0);
  tft.print("h ");
  tft.print(OBJECT_RA_M,2);
  tft.println("m");
  tft.print("DEC:");
  tft.print(OBJECT_DEC_D,0);
  tft.setTextSize(1);
  tft.print("o ");
  tft.setTextSize(2);
  tft.print(OBJECT_DEC_M,2);
  tft.println("'");
// End data for the observed object...
}else{
  tft.setTextSize(2);
  tft.setTextColor(texts);
  tft.println("");
  tft.println("No object selected!");
  tft.setTextSize(1);
  tft.println("Use LOAD button below to select");
  tft.println("from Messier Catalogue, from File,");
  tft.println("or from NGC Catalogue.");
}

  tft.drawLine(0, 275, 240, 275, btn_d_border);
  tft.drawLine(0, 276, 240, 276, btn_d_border);
  tft.drawLine(0, 277, 240, 277, btn_l_border);

//  Draw Bottom BUTTONS:

  tft.setTextSize(2);
  tft.setTextColor(btn_l_text);
if (IS_BT_MODE_ON == false){
  tft.drawRect(1,280,78,57, btn_l_border);
  tft.setCursor(15, 300);
  tft.println("LOAD");
  tft.drawRect(81,280,78,57, btn_l_border);
  tft.setCursor(97, 292);
  tft.println("GoTo");
  tft.setCursor(97, 310);
  tft.println("Home");
  tft.drawRect(1,339,78,57, btn_l_border);
  tft.setCursor(10, 360);
  tft.println("INPUT");
  tft.drawRect(81,339,78,57, btn_l_border);
  tft.setCursor(103, 360);
  tft.println("...");
} else{
  tft.drawRect(1,280,78,57, btn_d_border);
  tft.drawRect(81,280,78,57, btn_d_border);
  tft.drawRect(1,339,78,57, btn_d_border);
  tft.drawRect(81,339,78,57, btn_d_border);
}
  if (IS_TRACKING == true){
    tft.fillRect(162,281,76,55, btn_d_border);
  }       
  tft.drawRect(161,280,78,57, btn_l_border);
  tft.setCursor(172, 300);
  tft.println("TRACK");

  if (IS_BT_MODE_ON == true){
      tft.fillRect(162,340,76,55, btn_d_border);
  }       
  tft.drawRect(161,339,78,57, btn_l_border);
  tft.setCursor(177, 353);
  tft.println("Blue");
  tft.setCursor(170, 370);
  tft.println("Tooth");
}

void drawClockScreen(){
  CURRENT_SCREEN = 1;
  tft.fillScreen(BLACK);
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(20, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Date & Time");
  tft.setTextColor(d_text);
  tft.setCursor(30, 55);
  tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
//  tft.print(returnDate(0));
  tft.setCursor(80, 100);
  tft.print(rtc.getTimeStr(FORMAT_SHORT));
//  tft.print(returnTime());
  
  // Draw keypad....
  tft.setTextColor(btn_l_text);
  int kk=1;
  for (int i=0; i<4; i++){
    for (int j=0; j<3; j++){
      if (kk==10){
        if (Summer_Time == 1){
          tft.drawRect(11,336,71,56, btn_l_border);
          tft.fillRect(12,337,69,54, btn_d_border);
        }else{
          tft.setTextColor(btn_d_text);
        }
        tft.setTextSize(1);
        tft.setCursor(32, 352);
        tft.println("SUMMER");
        tft.setCursor(37, 365);
        tft.println("TIME");
        tft.setTextColor(btn_l_text);
        tft.setTextSize(3);
      }else if (kk==11){
        tft.fillRect(86,336,71,56, btn_d_border);
        tft.setCursor(112, 352);
        tft.println("0");
      }else if (kk==12){
        tft.drawRect(161,336,71,56, btn_l_border);
        tft.setCursor(180, 352);
        tft.println("OK");
      }else{
        tft.fillRect(((j*75)+11),((i*60)+156),71,56, btn_d_border);
        tft.setCursor(((j*75)+37), ((i*60)+172));
        tft.print(kk);
      }
      kk+=1;
    }
  }
}

void TimerUpdateDraw(int z){
   tft.setTextColor(d_text);
   w_DateTime[dateEntryPos] = z;
   if (dateEntryPos >=0 & dateEntryPos <2){
      tft.fillRect((dateEntryPos*18)+30,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+30,55);
   }else if (dateEntryPos >1 & dateEntryPos <4){
      tft.fillRect((dateEntryPos*18)+48,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+48,55);
   }else if (dateEntryPos >3 & dateEntryPos <8){
      tft.fillRect((dateEntryPos*18)+66,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+66,55);
   }else if (dateEntryPos >7 & dateEntryPos <10){
      tft.fillRect(((dateEntryPos-8)*18)+80,100,18,22, BLACK);
      tft.setCursor(((dateEntryPos-8)*18)+80,100);
   } else if (dateEntryPos >9){
      tft.fillRect(((dateEntryPos-8)*18)+98,100,18,22, BLACK);
      tft.setCursor(((dateEntryPos-8)*18)+98,100);
   }
   tft.print(w_DateTime[dateEntryPos]);
   if (dateEntryPos > 10){
     dateEntryPos = 0;
   }else{
     dateEntryPos += 1;
   }
}

void drawLoadScreen(){
  CURRENT_SCREEN = 6;
//  MESS_PAGER = 0;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Load...");

// Draw buttons to load CSVs
  tft.setTextSize(2);
  tft.setTextColor(btn_l_text);
  if (LOAD_SELECTOR == 1){
    tft.fillRect(1,40,117,30, btn_d_border);
  }
  tft.drawRect(1,40,117,30, btn_l_border);
  tft.setCursor(17, 48);
  tft.println("Messier");
  if (LOAD_SELECTOR == 2){
    tft.fillRect(120,40,119,30, btn_d_border);
  }
  tft.drawRect(120,40,119,30, btn_l_border);
  tft.setCursor(125, 48);
  tft.println("Treasures");

  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  tft.fillRect(160,5,70,25, btn_d_border);
  tft.drawRect(160,5,70,25, btn_l_border);
  tft.setCursor(163, 10);
  tft.println("<back");

  tft.drawLine(0, 353, 240, 353, btn_d_border);
  tft.drawLine(0, 354, 240, 354, btn_d_border);
  tft.drawLine(0, 355, 240, 355, btn_l_border);

//  tft.fillRect(5,360,80,35, btn_d_border);
  tft.drawRect(5,360,80,35, btn_l_border);
  tft.setCursor(15, 370);
  tft.println("<prev");

//  tft.fillRect(157,360,80,35, btn_d_border);
  tft.drawRect(157,360,80,35, btn_l_border);
  tft.setCursor(167, 370);
  tft.println("next>");
  
   if (LOAD_SELECTOR == 1){
   // I'll draw 15 objects per page, thus "(pager*15)" will give me the start of the [index_]
   int kk = MESS_PAGER*15;
   
      for (int i=0; i<5; i++){
          for (int j=0; j<3; j++){
            String M_NAME = Messier_Array[kk].substring(0,Messier_Array[kk].indexOf(';'));
          if (M_NAME == ""){
            break;
          }
          tft.fillRect(((j*75)+9),((i*50)+85),71,45, messie_btn);
          tft.setCursor(((j*75)+22), ((i*50)+101));
          tft.print(M_NAME);
          kk += 1;
          }
      }
   }else if (LOAD_SELECTOR == 2){
    tft.setTextSize(1);
    int ll = TREAS_PAGER*15;
   
      for (int i=0; i<5; i++){
          for (int j=0; j<3; j++){
            String M_NAME = Treasure_Array[ll].substring(0,Treasure_Array[ll].indexOf(';'));
          if (M_NAME == ""){
            break;
          }
          tft.fillRect(((j*75)+9),((i*50)+85),71,45, messie_btn);
          tft.setCursor(((j*75)+22), ((i*50)+101));
          tft.print(M_NAME);
          ll += 1;
          }
      }
  }
}

void drawStarSyncScreen(){
  CURRENT_SCREEN = 12;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Sync...");

  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);

  tft.fillRect(160,5,70,25, btn_d_border);
  tft.drawRect(160,5,70,25, btn_l_border);
  tft.setCursor(163, 10);
  tft.println("SKIP>");

  tft.drawLine(0, 353, 240, 353, btn_d_border);
  tft.drawLine(0, 354, 240, 354, btn_d_border);
  tft.drawLine(0, 355, 240, 355, btn_l_border);

//  tft.fillRect(5,360,80,35, btn_d_border);
  tft.drawRect(5,360,80,35, btn_l_border);
  tft.setCursor(15, 370);
  tft.println("<prev");

//  tft.fillRect(157,360,80,35, btn_d_border);
  tft.drawRect(157,360,80,35, btn_l_border);
  tft.setCursor(167, 370);
  tft.println("next>");
  
   // I'll draw 15 objects per page, thus "(pager*15)" will give me the start of the [index_]
   int kk = STARS_PAGER*15;
   for (int i=0; i<5; i++){
      for (int j=0; j<3; j++){
          int i1 = Stars[kk].indexOf(';');
          int i2 = Stars[kk].indexOf(';',i1+1);
          String S_NAME = Stars[kk].substring(i1+1, i2);
          String C_NAME = Stars[kk].substring(0, i1);
          if (S_NAME == ""){
            break;
          }
          tft.fillRect(((j*75)+9),((i*50)+85),71,45, messie_btn);
          int l = (S_NAME.length()/2)*6;
          tft.setCursor(((j*75)+(44-l)), ((i*50)+93));
          tft.setTextSize(1);
          tft.print(S_NAME);
          tft.setTextSize(2);
          tft.setCursor(((j*75)+29), ((i*50)+110));
          tft.print(C_NAME);
          kk += 1;
      }
   }
}

void drawGPSScreen(){
  CURRENT_SCREEN = 0;
  tft.fillScreen(BLACK);
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(20, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("GPS Location");
  tft.setTextColor(d_text);
  tft.setCursor(20, 55);
  tft.print("Waiting for");
  tft.setCursor(10, 85);
  tft.print("GPS satelite");
  tft.setTextSize(1);
  tft.setCursor(5, 120);
  tft.print("Once the GPS connects to satelites, it will calculate the LAT and LNG of your current position and update the Greenwich Time (GMT).");
 
  // Draw SKIP BTN
  tft.drawRect(44,315,150,60, btn_l_border);
  tft.setTextSize(2);
  tft.setTextColor(btn_l_text);
  tft.setCursor(100, 325);
  tft.println("SKIP");
  tft.setTextSize(1);
  tft.setCursor(50, 345);
  tft.print("Will assign coordinates");
  tft.setCursor(68, 355);
  tft.print("for Sofia, Bulgaria");
}

void Sidereal_rate(){
  // when a manual movement of the drive happens. - This will avoid moving the stepepers with a wrong Step Mode.
  if ((IS_MANUAL_MOVE == false)&(IS_TRACKING == true)){
    setmStepsMode("R",MICROSteps);
    digitalWrite(RA_DIR,STP_BACK);
    digitalWrite(RA_STP,HIGH);
    Serial.println("RA_microSteps changed by: SiderealRate -=1");
    RA_microSteps -= 1;
    digitalWrite(RA_STP,LOW);
  }
}

void cosiderSlewTo(){
//  int RA_microSteps, DEC_microSteps;
//  int SLEW_RA_microsteps, SLEW_DEC_microsteps;
//  INT data type -> -2,147,483,648 to 2,147,483,647 
//  max count of microsteps x32 for full 360 rotation =  1,382,400 microsteps in my case
//  for more details see the XLS file with calculations
//...

if (IS_OBJ_FOUND == false){
    float HAH;
    float  HAM;
    float DECD;
    float DECM;
    double HA_decimal, DEC_decimal;

    // Stop Interrupt procedure for tracking.
    Timer3.stop(); // 
    IS_TRACKING = false;

    if (HAHour >= 12){
        HAH = HAHour - 12;
        HAM = HAMin;
        IS_MERIDIAN_PASSED = false;
    }else{
        HAH = HAHour;
        HAM = HAMin;
        IS_MERIDIAN_PASSED = true;
    }

    HA_decimal = (HAH+(HAM/60))*15;   // In degrees - decimal
    DEC_decimal = OBJECT_DEC_D + (OBJECT_DEC_M/60); // In degrees - decimal

    SLEW_RA_microsteps  = HA_decimal * HA_H_CONST;     // Hardware Specific Code  
    SLEW_DEC_microsteps = DEC_90 - (DEC_decimal * DEC_D_CONST);    // Hardware specific code
//    SLEW_RA_microsteps  = int(HAH*HA_H_CONST)+int(HAM*HA_M_CONST);     // Hardware Specific Code  
//    SLEW_DEC_microsteps = DEC_90 - (int(OBJECT_DEC_D*DEC_D_CONST)+ int(OBJECT_DEC_M*DEC_M_CONST));    // Hardware specific code
    if(IS_MERIDIAN_PASSED == true){
        SLEW_DEC_microsteps*= -1;
    }

    // ADJUST MICROSTEPS ACCORDING THE ALIGNMENT
          SLEW_RA_microsteps *= DELTA_RA_ADJUST;
          SLEW_DEC_microsteps *= DELTA_DEC_ADJUST;

// If Home Position selected .... Make sure it goes to 0.
    int home_pos = 0;
    if ((OBJECT_RA_H == 12) & (OBJECT_RA_M == 0) & (OBJECT_DEC_D == 90) & (OBJECT_DEC_M == 0)){
        SLEW_RA_microsteps = RA_90;
        SLEW_DEC_microsteps = 0;
        home_pos = 1;
    }

// Make the motors START slow and then speed-up - using the microsteps!
// Speed goes UP in 2.2 sec....then ..... FULL Speed ..... then....Slpeed goes Down for 3/4 Revolution of the drive  
int delta_DEC_time = millis() - Slew_timer;
int delta_RA_timer = millis() - Slew_RA_timer;

    if (delta_DEC_time >= 0 & delta_DEC_time < 900){
            setmStepsMode("D",8);
    }
    if (delta_DEC_time >= 900 & delta_DEC_time < 1800){
            setmStepsMode("D",4);
    }
    if (delta_DEC_time >= 1800 & delta_DEC_time < 2200){
            setmStepsMode("D",2);
    }
    if (delta_DEC_time >= 2200){
            setmStepsMode("D",1);
    }

    if (delta_RA_timer >= 0 & delta_RA_timer < 900){
            setmStepsMode("R",8);
    }
    if (delta_RA_timer >= 900 & delta_RA_timer < 1800){
            setmStepsMode("R",4);
    }
    if (delta_RA_timer >= 1800 & delta_RA_timer < 2200){
            setmStepsMode("R",2);
    }
    if (delta_RA_timer >= 2200){
            setmStepsMode("R",1);
    }

    int delta_RA_steps = SLEW_RA_microsteps - RA_microSteps;
    int delta_DEC_steps = SLEW_DEC_microsteps - DEC_microSteps;

    if ((abs(delta_DEC_steps) >= 1200)&(abs(delta_DEC_steps) <= 2400)){ 
            setmStepsMode("D",4);
    }
    if ((abs(delta_DEC_steps) < 1200)){ 
            setmStepsMode("D",8);
    }
    if ((abs(delta_RA_steps) >= 1200)&(abs(delta_RA_steps) <= 2400)){
            setmStepsMode("R",4);

    }
    if (abs(delta_RA_steps) < 1200){
            setmStepsMode("R",8);

    }

    // Taking care of the RA Slew_To.... and make sure it ends Last
    // NB: This way we can jump to TRACK and be sure the RA is on target   
    if(abs(delta_RA_steps)>= abs(delta_DEC_steps)){
        if (RA_finish_last == 0){
          RA_finish_last = 1;
          Slew_RA_timer = millis();
        }
    }
//    Serial.println(Slew_RA_timer);
    if ((IS_OBJECT_RA_FOUND == false)&(RA_finish_last == 1)){    
            if (SLEW_RA_microsteps >= (RA_microSteps-RA_mode_steps) & SLEW_RA_microsteps <= (RA_microSteps+RA_mode_steps)){
               IS_OBJECT_RA_FOUND = true;               
            } else {
                  if (SLEW_RA_microsteps > RA_microSteps){
                      digitalWrite(RA_DIR,STP_BACK);
                      digitalWrite(RA_STP,HIGH);
                      digitalWrite(RA_STP,LOW);
//                      Serial.println("RA_microSteps changed by: SlewTo += RA_mode_steps");
                      RA_microSteps += RA_mode_steps;
                  }else{
                      digitalWrite(RA_DIR,STP_FWD);
                      digitalWrite(RA_STP,HIGH);
                      digitalWrite(RA_STP,LOW);
//                      Serial.println("RA_microSteps changed by: SlewTo -= RA_mode_steps");
                      RA_microSteps -= RA_mode_steps;
                  } 
            }
    }

    // Taking care of the DEC Slew_To....  
    if (IS_OBJECT_DEC_FOUND == false){    
        if (SLEW_DEC_microsteps >= (DEC_microSteps-DEC_mode_steps) & SLEW_DEC_microsteps <= (DEC_microSteps+DEC_mode_steps)){
              IS_OBJECT_DEC_FOUND = true;
        } else {
              if (SLEW_DEC_microsteps > DEC_microSteps){
                  digitalWrite(DEC_DIR,STP_BACK);
                  digitalWrite(DEC_STP,HIGH);
                  digitalWrite(DEC_STP,LOW);
                  DEC_microSteps += DEC_mode_steps;
              }else{
                  digitalWrite(DEC_DIR,STP_FWD);
                  digitalWrite(DEC_STP,HIGH);
                  digitalWrite(DEC_STP,LOW);
                  DEC_microSteps -= DEC_mode_steps;
              } 
        }
    }

    // Check if Object is found on both Axes...
    if (IS_OBJECT_RA_FOUND == true & IS_OBJECT_DEC_FOUND == true){
      Serial.println("Both RA and DEC are found....");
      Serial.print("Current RA microsteps :");
      Serial.print(RA_microSteps);
      Serial.print(" --> Current HA:");
      Serial.print(HAHour);
      Serial.print("h ");
      Serial.println(HAMin);
      Serial.print("Current DEC microsteps :");
      Serial.print(DEC_microSteps);
      Serial.print(" --> Current DEC:");
      Serial.print(OBJECT_DEC_D);
      Serial.print("* ");
      Serial.println(OBJECT_DEC_M);
      IS_OBJ_FOUND = true;
      if ((home_pos == 0 )&(ALT > 0)&(IS_IN_OPERATION == true)){
          IS_TRACKING = true;
          setmStepsMode("R",MICROSteps);
          Timer3.start(Clock_Motor);
      }
      SoundOn(note_C,64);
      Slew_RA_timer = 0;
      RA_finish_last = 0;
      if (IS_BT_MODE_ON == true){
         Serial3.println("Slew done! Object in scope!");
      }
      if (IS_IN_OPERATION == true){
        drawMainScreen();
      }else{
        drawConstelationScreen(SELECTED_STAR);
      }
    }
 }
}

void considerBTCommands(){
    // Check if the commandString is complete - should START and END with "$" sign
    if (BT_COMMAND_STR.charAt(0)=='$' & BT_COMMAND_STR.charAt(BT_COMMAND_STR.length()-1) == '$'){
        // String command is complete!
        BT_COMMAND_STR = BT_COMMAND_STR.substring(1,BT_COMMAND_STR.length()-1);
        String COMMAND = BT_COMMAND_STR.substring(0,BT_COMMAND_STR.indexOf(';'));
        SoundOn(note_c,8);
        // Serial.print("Command=");
        // Serial.println(COMMAND);
        if (COMMAND == "Current"){
            Serial3.print("MicroSteps_360 = ");
            Serial3.println(MicroSteps_360);
            Serial3.print("ARCSEC_FULL_ROTATION = ");
            Serial3.println(ARCSEC_F_ROTAT);
            Serial3.print("RA_90 = ");
            Serial3.println(RA_90);
            Serial3.print("DEC_D_CONST = RA_D_CONST = ");
            Serial3.println(DEC_D_CONST);
            
            Serial3.print("Clock_Motor = ");
            Serial3.println(Clock_Motor);
            Serial3.print("Meridian Flip var = ");
            Serial3.println(mer_flp);
            Serial3.print("ALT = ");
            Serial3.println(int((asin(ALT)*180)/3.14159265));
            Serial3.print("IS_OBJ_FOUND = ");
            Serial3.println(IS_OBJ_FOUND);
            Serial3.print("HA = ");
            Serial3.print(HAHour,0);
            Serial3.print("h ");
            Serial3.print(HAMin);
            Serial3.print("m      | current RA_microSteps: ");
            Serial3.println(RA_microSteps);
            Serial3.print("DEC = ");
            Serial3.print(OBJECT_DEC_D,0);
            Serial3.print("* ");
            Serial3.print(OBJECT_DEC_M);
            Serial3.print("m      | current DEC_microSteps: ");
            Serial3.print(DEC_microSteps);
        }else if (COMMAND == "SlewTo"){
           // Serial.println("SlewTo Command");
           int i1 = BT_COMMAND_STR.indexOf(';');
           int i2 = BT_COMMAND_STR.indexOf(';',i1+1);
           int i3 = BT_COMMAND_STR.indexOf(';',i2+1);
           int i4 = BT_COMMAND_STR.indexOf(';',i3+1);
           int i5 = BT_COMMAND_STR.indexOf(';',i4+1);
           int i6 = BT_COMMAND_STR.indexOf(';',i5+1);
           int i7 = BT_COMMAND_STR.indexOf(';',i6+1);
           int i8 = BT_COMMAND_STR.indexOf(';',i7+1);
           int i9 = BT_COMMAND_STR.indexOf(';',i8+1);
           int i10 = BT_COMMAND_STR.indexOf(';',i9+1);
           int i11 = BT_COMMAND_STR.indexOf(';',i10+1);
           OBJECT_NAME = BT_COMMAND_STR.substring(i1+1, i2);
           OBJECT_DESCR = "Pushed via BlueTooth";
           OBJECT_DETAILS = "The "+OBJECT_NAME+" is a type "+BT_COMMAND_STR.substring(i10+1, i11)+" object in constellation "+BT_COMMAND_STR.substring(i9+1, i10);
           OBJECT_DETAILS += ", with visible magnitude of "+BT_COMMAND_STR.substring(i7+1, i8)+" and size of "+BT_COMMAND_STR.substring(i6+1, i7)+"'. ";
           OBJECT_DETAILS += OBJECT_NAME+" is "+ BT_COMMAND_STR.substring(i8+1, i9)+"mil. l.y from Earth";
           if (BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length()) != "0" & BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length()) != ""){
              OBJECT_DETAILS += " and is a.k.a "+BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length())+".";
           }
           OBJECT_RA_H = BT_COMMAND_STR.substring(i2+1, i3).toFloat();
           OBJECT_RA_M = BT_COMMAND_STR.substring(i3+1, i4).toFloat();
           OBJECT_DEC_D = BT_COMMAND_STR.substring(i4+1, i5).toFloat();
           OBJECT_DEC_M = BT_COMMAND_STR.substring(i5+1, i6).toFloat();
           
           // Write down the Observed objects information: --- USED in the STATS screen and sent to BT as status.
           int Delta_Time = (((String(rtc.getTimeStr()).substring(0,2).toInt())*60)  + (String(rtc.getTimeStr()).substring(3,5).toInt())) - ((Prev_Obj_Start.substring(0,2).toInt() * 60) + Prev_Obj_Start.substring(3).toInt());
           if (Delta_Time < 0){
              Delta_Time += 1440;
           }
           ObservedObjects[Observed_Obj_Count-1]+= String(Delta_Time)+";"+int(HAHour)+"h "+HAMin+"m;"+int((asin(ALT)*180)/3.14159265);
           ObservedObjects[Observed_Obj_Count]= OBJECT_NAME+";"+OBJECT_DETAILS+";"+String(rtc.getTimeStr()).substring(0,5)+";"+int(_temp)+";"+int(_humid)+";";
           Observed_Obj_Count +=1;
           Prev_Obj_Start = String(rtc.getTimeStr()).substring(0,5);

           // Now SlewTo the selected object and draw information on mainScreen
           calculateLST_HA();
            if (ALT > 0){
                SoundOn(note_C,32);
                delay(200);
                SoundOn(note_C,32);
                delay(200);
                SoundOn(note_C,32);
                delay(1500);
                UpdateObservedObjects();
                IS_OBJ_FOUND = false;
                IS_OBJECT_RA_FOUND = false;
                IS_OBJECT_DEC_FOUND = false;
                Slew_timer = millis();
                Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            }
           drawMainScreen();
        }else if (COMMAND == "Status"){
           double st;
           int st_h;
           int st_m;
            st = (String(rtc.getTimeStr()).substring(0,2).toInt()*60) + String(rtc.getTimeStr()).substring(3,5).toInt();
            st -= (START_TIME.substring(0, 2).toInt() * 60) + START_TIME.substring(3, 5).toInt();
           if (st < 0){
             st += 1440;
           }
           st_h = int(st/60);
           st_m = ((st/60) - st_h)*60;

           Serial3.println("rDUINO SCOPE - TELESCOPE GOTO System");  
           Serial3.println("Copyright (C) 2016 Dessislav Gouzgounov\n\n");
           Serial3.println("Download for free @ http://rduinoscope.co.nf");
           Serial3.print("Statistics for the observation on ");  
           Serial3.print(Start_date);
           Serial3.print(" @");
           Serial3.print(START_TIME);
           Serial3.println("h ");
           Serial3.print("Report generated at: ");
           Serial3.println(rtc.getTimeStr());
           Serial3.println("===============================================");
           Serial3.println("Data about location:"); 
           Serial3.print("      LATITUDE: ");
           Serial3.println(OBSERVATION_LONGITUDE, 4);
           Serial3.print("      LONGITUDE: ");
           Serial3.println(OBSERVATION_LATTITUDE, 4);
           Serial3.print("      ALTITUDE: ");
           Serial3.print(OBSERVATION_ALTITUDE, 0);
           Serial3.println();
           Serial3.print("Observation started at ");
           Serial3.print(START_TIME);
           Serial3.print("h, and continued for ");
           Serial3.print(st_h);
           Serial3.print("h ");
           Serial3.print(st_m);
           Serial3.print("m. Environmental temperature was ");
           Serial3.print(_temp, 0);
           Serial3.print("C and humidity was ");
           Serial3.print(_humid, 0);
           Serial3.println("%");
           Serial3.println();
           Serial3.print("Observed the following ");
           Serial3.print(Observed_Obj_Count);
           Serial3.println(" object(s):");
           Serial3.println("");
           for (int i=0; i<Observed_Obj_Count; i++){
                int i1 = ObservedObjects[i].indexOf(';');
                int i2 = ObservedObjects[i].indexOf(';',i1+1);
                int i3 = ObservedObjects[i].indexOf(';',i2+1);
                int i4 = ObservedObjects[i].indexOf(';',i3+1);
                int i5 = ObservedObjects[i].indexOf(';',i4+1);
                int i6 = ObservedObjects[i].indexOf(';',i5+1);
                int i7 = ObservedObjects[i].indexOf(';',i6+1);
                int tt;
                String ha_;
                String degs_;
                if (i == (Observed_Obj_Count-1)){
                    tt = (((String(rtc.getTimeStr()).substring(0,2).toInt())*60)  + (String(rtc.getTimeStr()).substring(3,5).toInt())) - ((ObservedObjects[i].substring(i2+1, i2+3).toInt() * 60) + ObservedObjects[i].substring(i2+4, i3).toInt());
                    if (tt < 0){
                       tt += 1440;
                    }
                }else{
                    tt = ObservedObjects[i].substring(i7+1, ObservedObjects[i].length()).toInt();
                }
              degs_ = ObservedObjects[i].substring(i6+1,i7);
              ha_ = ObservedObjects[i].substring(i5+1,i6);
              String Composed = "@ "+ ObservedObjects[i].substring(i2+1, i3) + "h, " + ObservedObjects[i].substring(0, i1) + " was observed for " + String(tt) + " min";
              Composed += "\nAt the time of observation the object was "+degs_+"deg. above horizon, with HA:"+ha_+" Environment wise: " + ObservedObjects[i].substring(i3+1, i4) + " C and " + ObservedObjects[i].substring(i4+1, i5) + "% humidity. "+ObservedObjects[i].substring(i1+1, i2)+"\n";
              Serial3.println(Composed);
            }
        }  
    }
    BT_COMMAND_STR = "";
    Serial3.flush();
}

void consider_Manual_Move(int xP, int yP){
    if ((xP > 0) & (xP < 150)){
      setmStepsMode("R",1);
      digitalWrite(RA_DIR,STP_BACK);
      digitalWrite(RA_STP,HIGH);
      digitalWrite(RA_STP,LOW);
      RA_microSteps += RA_mode_steps;
    }else if ((xP > 150) & (xP <= 500)){
      setmStepsMode("R",4);
      digitalWrite(RA_DIR,STP_BACK);
      digitalWrite(RA_STP,HIGH);
      digitalWrite(RA_STP,LOW);
      RA_microSteps += RA_mode_steps;
    }else if ((xP > 800) & (xP <= 990)){
      setmStepsMode("R",4);
      digitalWrite(RA_DIR,STP_FWD);
      digitalWrite(RA_STP,HIGH);
      digitalWrite(RA_STP,LOW);
      RA_microSteps -= RA_mode_steps;
    }else if ((xP > 990) & (xP < 1024)){
      setmStepsMode("R",1);
      digitalWrite(RA_DIR,STP_FWD);
      digitalWrite(RA_STP,HIGH);
      digitalWrite(RA_STP,LOW);
      RA_microSteps -= RA_mode_steps;
    }

    if ((yP > 0) & (yP < 150)){
      setmStepsMode("D",1);
      digitalWrite(DEC_DIR,STP_BACK);
      digitalWrite(DEC_STP,HIGH);
      digitalWrite(DEC_STP,LOW);
      DEC_microSteps += DEC_mode_steps;
    }else if ((yP > 150) & (yP <= 500)){
      setmStepsMode("D",4);
      digitalWrite(DEC_DIR,STP_BACK);
      digitalWrite(DEC_STP,HIGH);
      digitalWrite(DEC_STP,LOW);
      DEC_microSteps += DEC_mode_steps;
    }else if ((yP > 800) & (yP <= 990)){
      setmStepsMode("D",4);
      digitalWrite(DEC_DIR,STP_FWD);
      digitalWrite(DEC_STP,HIGH);
      digitalWrite(DEC_STP,LOW);
      DEC_microSteps -= DEC_mode_steps;
    }else if ((yP > 990) & (yP < 1024)){
      setmStepsMode("D",1);
      digitalWrite(DEC_DIR,STP_FWD);
      digitalWrite(DEC_STP,HIGH);
      digitalWrite(DEC_STP,LOW);
      DEC_microSteps -= DEC_mode_steps;
    }
}

// Keep the GPS sensor "fed" until we find the data.
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  SoundOn(note_c,8);
  do 
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}

void considerDayNightMode(){
  boolean prev_night_mode = IS_NIGHTMODE;
          if (analogRead(A3) < 800){
              IS_NIGHTMODE = true;
          } else {
              IS_NIGHTMODE = false;
          }
         if (IS_NIGHTMODE == true){   // Night Mode
            texts = Maroon;
            l_text = RED;
            d_text = Maroon;
            title_bg = Maroon;
            title_texts = BLACK;
            messie_btn = Maroon;
            btn_l_text = RED;
            btn_d_text = Maroon;
            btn_l_border = RED;
            btn_d_border = Maroon;
            btn_l_selection = RED;
            MsgBox_bg = RED;
            MsgBox_t = BLACK;
         }else{     // Day Mode
            texts = LightGrey;
            l_text = WHITE;
            d_text = LightGrey;
            btn_l_text = GreenYellow;
            btn_d_text = DarkGreen;
            btn_l_border = GREEN;
            btn_d_border = DarkGreen;
            btn_l_selection = DarkGreen;
            title_bg = Orange;
            title_texts = BLACK;
            messie_btn = DarkGrey;
            MsgBox_bg = Purple;
            MsgBox_t = GreenYellow;
         } 
      // Now make sure it works for all screen - redraws them when mode switched  
      if (prev_night_mode != IS_NIGHTMODE){
           if (CURRENT_SCREEN == 0){
                drawGPSScreen();
           }else if (CURRENT_SCREEN == 1){
                drawClockScreen();
           }else if (CURRENT_SCREEN == 2){
                //    drawPolarAlignScreen();
           }else if (CURRENT_SCREEN == 4){
                drawMainScreen();
           }else if (CURRENT_SCREEN == 6){
                drawLoadScreen();
           }
           else if (CURRENT_SCREEN == 10){
                drawSTATScreen();
           }
     }
 }

void drawSTATScreen(){
  CURRENT_SCREEN = 10;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Stats");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  tft.fillRect(160,5,70,25, btn_d_border);
  tft.drawRect(160,5,70,25, btn_l_border);
  tft.setCursor(163, 10);
  tft.println("<back");

  // Draw staistics...
  tft.setTextColor(l_text);
  tft.setTextSize(1);
  tft.setCursor(0, 40);
           tft.print("Observation on ");  
           tft.print(Start_date);
           tft.print(" @");
           tft.print(START_TIME);
           tft.println("h"); 
           tft.print("Report generated at: ");
           tft.println(String(rtc.getTimeStr()).substring(0,5));
           tft.println(" ");
           tft.println("  LOCATION:"); 
           tft.print("LAT : ");
           tft.println(OBSERVATION_LATTITUDE, 4);
           tft.print("LONG: ");
           tft.println(OBSERVATION_LONGITUDE, 4);
           tft.print("ALT : ");
           tft.println(OBSERVATION_ALTITUDE, 0);
           tft.println("");
           tft.print("  OBSERVATION: ");
           double st;
           int st_h;
           int st_m;
            st = (String(rtc.getTimeStr()).substring(0,2).toInt()*60) + String(rtc.getTimeStr()).substring(3,5).toInt();
            st -= (START_TIME.substring(0, 2).toInt() * 60) + START_TIME.substring(3, 5).toInt();
           if (st < 0){
             st += 1440;
           }
           st_h = int(st/60);
           st_m = ((st/60) - st_h)*60;
           tft.print(st_h);
           tft.print("h ");
           tft.print(st_m);
           tft.println("m");
           tft.print("Observation started at ");
           tft.print(START_TIME);
           tft.print("h where environment tempersture was ");
           tft.print(_Stemp, 0);
           tft.print("C and ");
           tft.print(_Shumid, 0);
           tft.println("% humidity\n");
           tft.print("  OBJECTS: ");
           tft.print(Observed_Obj_Count);
           tft.println(" object(s) observed:");
           tft.println("");
           for (int i=0; i<Observed_Obj_Count; i++){
                int i1 = ObservedObjects[i].indexOf(';');
                int i2 = ObservedObjects[i].indexOf(';',i1+1);
                int i3 = ObservedObjects[i].indexOf(';',i2+1);
                int i4 = ObservedObjects[i].indexOf(';',i3+1);
                int i5 = ObservedObjects[i].indexOf(';',i4+1);
                int i6 = ObservedObjects[i].indexOf(';',i5+1);
                int i7 = ObservedObjects[i].indexOf(';',i6+1);
                int tt;
                String ha_;
                String degs_;
                if (i == (Observed_Obj_Count-1)){
                    tt = (((String(rtc.getTimeStr()).substring(0,2).toInt())*60)  + (String(rtc.getTimeStr()).substring(3,5).toInt())) - ((ObservedObjects[i].substring(i2+1, i2+3).toInt() * 60) + ObservedObjects[i].substring(i2+4, i3).toInt());
                    if (tt < 0){
                       tt += 1440;
                    }
                }else{
                    tt = ObservedObjects[i].substring(i7+1, ObservedObjects[i].length()).toInt();
                }
              degs_ = ObservedObjects[i].substring(i6+1,i7);
              ha_ = ObservedObjects[i].substring(i5+1,i6);
              String Composed = ObservedObjects[i].substring(0, i1) +" @"+ ObservedObjects[i].substring(i2+1, i3) + " for " + String(tt) + "min |Alt:"+degs_+"deg\n";
              //Composed += "\nAt the time of observation the object was "+degs_+"deg. above horizon, with HA:"+ha_+" Environment wise: " + ObservedObjects[i].substring(i3+1, i4) + " C and " + ObservedObjects[i].substring(i4+1, i5) + "% humidity. ";
              tft.print(Composed);
            }
}

void setmStepsMode(char* P, int mod){    // P means the direction like RA or DEC; mod means MicroSteppping mode: x32, x16, x8....
// setmStepsMode(R,2) - means RA with 1/2 steps; setmStepsMode(R,4) - means RA with 1/4 steps
  if (P=="R"){  // Set RA modes
    if (mod == 1){                      // Full Step
        digitalWrite(RA_MODE0, LOW);
        digitalWrite(RA_MODE1, LOW);
        digitalWrite(RA_MODE2, LOW);
    }
    if (mod == 2){                      // 1/2 Step
        digitalWrite(RA_MODE0, HIGH);
        digitalWrite(RA_MODE1, LOW);
        digitalWrite(RA_MODE2, LOW);
    }
    if (mod == 4){                      // 1/4 Step
        digitalWrite(RA_MODE0, LOW);
        digitalWrite(RA_MODE1, HIGH);
        digitalWrite(RA_MODE2, LOW);
    }
    if (mod == 8){                      // 1/8 Step
        digitalWrite(RA_MODE0, HIGH);
        digitalWrite(RA_MODE1, HIGH);
        digitalWrite(RA_MODE2, LOW);
    }
    if (mod == 16){                      // 1/16 Step
        digitalWrite(RA_MODE0, LOW);
        digitalWrite(RA_MODE1, LOW);
        digitalWrite(RA_MODE2, HIGH);
    }
    if (mod == 32){                      // 1/32 Step
        digitalWrite(RA_MODE0, HIGH);
        digitalWrite(RA_MODE1, LOW);
        digitalWrite(RA_MODE2, HIGH);
    }
    RA_mode_steps = MICROSteps/mod;
  }
  if (P=="D"){  // Set RA modes
    if (mod == 1){                      // Full Step
        digitalWrite(DEC_MODE0, LOW);
        digitalWrite(DEC_MODE1, LOW);
        digitalWrite(DEC_MODE2, LOW);
    }
    if (mod == 2){                      // 1/2 Step
        digitalWrite(DEC_MODE0, HIGH);
        digitalWrite(DEC_MODE1, LOW);
        digitalWrite(DEC_MODE2, LOW);
    }
    if (mod == 4){                      // 1/4 Step
        digitalWrite(DEC_MODE0, LOW);
        digitalWrite(DEC_MODE1, HIGH);
        digitalWrite(DEC_MODE2, LOW);
    }
    if (mod == 8){                      // 1/8 Step
        digitalWrite(DEC_MODE0, HIGH);
        digitalWrite(DEC_MODE1, HIGH);
        digitalWrite(DEC_MODE2, LOW);
    }
    if (mod == 16){                      // 1/16 Step
        digitalWrite(DEC_MODE0, LOW);
        digitalWrite(DEC_MODE1, LOW);
        digitalWrite(DEC_MODE2, HIGH);
    }
    if (mod == 32){                      // 1/32 Step
        digitalWrite(DEC_MODE0, HIGH);
        digitalWrite(DEC_MODE1, LOW);
        digitalWrite(DEC_MODE2, HIGH);
    }
    DEC_mode_steps = MICROSteps/mod;
  }
}

void SoundOn(int note, int duration){
  duration *= 10000;
  long elapsed_time = 0;
  while (elapsed_time < duration) {
      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(note / 2);
      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(note / 2);
      // Keep track of how long we pulsed
      elapsed_time += (note);
  } 
}

void OnScreenMsg(int Msg){
// Msg = 1 -> Moving;
// Msg = 2 -> Tracking Off;
  String m1, m2, m3;
  tft.fillRect(40, 150, 160, 100, MsgBox_bg );
  tft.drawRect(42, 152, 155, 96, MsgBox_t);
  tft.setTextColor(MsgBox_t);
  if (Msg == 1){
    m1 = "MOVING";
    m2 = "Step Back";
    m3 = "PLEASE!";
    tft.setCursor(67, 165);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(72, 205);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(82, 225);
    tft.print(m3);
  }else if (Msg == 2){
    m1 = "WARNING";
    m2 = "TRACKING was";
    m3 = "Turned Off!";
    //Timer3.stop(); // 
    //IS_TRACKING = false;
    tft.setCursor(58, 165);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(48, 205);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(58, 225);
    tft.print(m3);
  }
}

void drawCoordinatesScreen(){
  CURRENT_SCREEN = 5;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Coords");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  tft.fillRect(160,5,70,25, btn_d_border);
  tft.drawRect(160,5,70,25, btn_l_border);
  tft.setCursor(163, 10);
  tft.println("<back");

  // Draw Coordinates screen...
  tft.setTextColor(l_text);
  tft.setTextSize(2);
  tft.setCursor(5, 60);
  tft.print("HA : ");
  tft.setCursor(5, 90);
  tft.print("RA : ");
  tft.setCursor(5, 120);
  tft.print("DEC: ");
  tft.fillRect(50,55,170,25, btn_d_border);
  tft.drawRect(51,56,169,23, btn_l_border);
  tft.fillRect(50,85,170,25, btn_d_border);
  tft.drawRect(51,86,169,23, btn_l_border);
  tft.fillRect(50,115,170,25, btn_d_border);
  tft.drawRect(51,116,169,23, btn_l_border);
  tft.setCursor(5, 165);
  tft.setTextSize(1);
  tft.print("Coordinates represented here are based on the current motor positions and re-calculated back to Hour Angle, Right Ascension and Declination");
}

void drawConstelationScreen(int indx){
  CURRENT_SCREEN = 13;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Alignment");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);

  // Draw the constellation image
  // 240 x 150 px for image of the constellation - starting from y=40px
  
  // Draw coordinates, MicroSteps and Corrected MicroSteps (when fine tuning the position of the star in eyepiece).
  // Basically the Corrected MicroSteps are the correction to apply to all coordinates from this moment on.
  tft.setCursor(0, 200);
  tft.println("");
  tft.println(OBJECT_NAME);
  tft.setTextSize(1);
  tft.print("HA : ");
  tft.print(HAHour,0);
  tft.print("h ");
  tft.print(HAMin);
  tft.println("m");
  tft.print("DEC: ");
  tft.print(OBJECT_DEC_D,0);
  tft.print("* ");
  tft.print(OBJECT_DEC_M);
  tft.println("m");
  tft.print("RA_microSteps: ");
  tft.println(SLEW_RA_microsteps);
  tft.print("DEC_microSteps: ");
  tft.println(SLEW_DEC_microsteps);
  tft.println("");
  tft.setTextSize(2);
  tft.print("dElta RA : ");
  tft.println(SLEW_RA_microsteps - RA_microSteps);
  tft.println("cos RA: ");
  tft.print("dElta DEC: ");
  tft.println(SLEW_DEC_microsteps - DEC_microSteps);
  tft.print("cos DEC: ");

  // Draw buttons : OK and Another StarAlignment
  tft.drawRect(5,360,100,35, btn_l_border);
  tft.setCursor(10, 370);
  tft.println("<Repeat");

  tft.drawRect(127,360,110,35, btn_l_border);
  tft.setCursor(147, 370);
  tft.println("ALIGN!");
  
}
void UpdateObservedObjects(){
   // Write down the Observed objects information: --- USED in the STATS screen and sent to BT as status.
   int Delta_Time = (((String(rtc.getTimeStr()).substring(0,2).toInt())*60)  + (String(rtc.getTimeStr()).substring(3,5).toInt())) - ((Prev_Obj_Start.substring(0,2).toInt() * 60) + Prev_Obj_Start.substring(3).toInt());
   if (Delta_Time < 0){
      Delta_Time += 1440;
   }
   ObservedObjects[Observed_Obj_Count-1]+= ";"+String(Delta_Time);
   ObservedObjects[Observed_Obj_Count]= OBJECT_NAME+";"+OBJECT_DETAILS+";"+String(rtc.getTimeStr()).substring(0,5)+";"+int(_temp)+";"+int(_humid)+";"+int(HAHour)+"h "+HAMin+"m;"+int((asin(ALT)*180)/3.14159265);
   Observed_Obj_Count +=1;
   Prev_Obj_Start = String(rtc.getTimeStr()).substring(0,5);
}

