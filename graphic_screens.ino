//
//    rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    PROJECT Website: http://rduinoscope.byethost24.com
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// ......................................................................
//  This part of the code take care of:
//  - Drawing interface screens;
//  - Day / Night mode set;
//  - Draws symbols on certain screens (e.g. XX/XX/XXXX in day/time screen)
//  - Draws OnScreen Messages
//
//  Screens are separated like:
//    * CURRENT_SCREEN==0  - drawGPSScreen() Where the GPS coordinates are displayed
//    * CURRENT_SCREEN==1  - drawClockScreen() Captures updates on the time and date
//    * CURRENT_SCREEN==2  -                - not used
//    * CURRENT_SCREEN==3  - drawSelectAlignment() Select Alignment method (only have 3 buttons)
//    * CURRENT_SCREEN==4  - drawMainScreen() Captures all clicks on the MAIN Screen of the application
//    * CURRENT_SCREEN==5  - drawCoordinatesScreen() Only have "back" button
//    * CURRENT_SCREEN==6  - drawLoadScreen() Captures input on Load screen (all of them: Messier && Treasurres)
//    * CURRENT_SCREEN==7  - drawOptionsScreen();
//    * CURRENT_SCREEN==8  -                - not used
//    * CURRENT_SCREEN==9  -                - not used
//    * CURRENT_SCREEN==10  - drawSTATScreen()
//    * CURRENT_SCREEN==11  - drawStarMap();              
//    * CURRENT_SCREEN==12  - drawStarSyncScreen() - To Select Alignment Star; 
//    * CURRENT_SCREEN==13  - drawConstelationScreen(int indx) - to actually align on Star. Called few times per alignment procedure.
//

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


void drawClockScreen(){
  CURRENT_SCREEN = 1;
  tft.fillScreen(BLACK);
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(20, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Date && Time");
  tft.setTextColor(l_text);
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
          DrawButton(11,336,71,56, "SUMMER", btn_d_border, btn_l_border, btn_l_text, 1);
        }else{
          DrawButton(11,336,71,56, "SUMMER", 0, btn_l_border, btn_l_text, 1);
        }
      }else if (kk==11){
        DrawButton(86,336,71,56, "0", btn_d_border, 0, btn_l_text, 3);
      }else if (kk==12){
        DrawButton(161,336,71,56, "OK", 0, btn_l_border, btn_l_text, 3);
      }else{
        DrawButton(((j*75)+11), ((i*60)+156), 71, 56, String(kk), btn_d_border, 0, btn_l_text, 3);
      }
      kk+=1;
    }
  }
}


void drawSelectAlignment(){
  CURRENT_SCREEN = 3;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Alignment");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);

  DrawButton(20,100,200,55, "1 Star Alignment", btn_d_border, btn_l_border, btn_l_text, 2);
//  DrawButton(20,170,200,55, "2 Star Alignment", btn_d_border, btn_l_border, btn_l_text, 2);
  if (OBSERVATION_LATTITUDE > 0){
      DrawButton(20,170,200,55, "Iterative Align", btn_d_border, btn_l_border, btn_l_text, 2);
  }
  DrawButton(20,320,200,45, "Skip Alignment", 0, btn_l_border, btn_l_text, 2);
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
    tft.print((LST - (int)LST)*60, 0);
  }else{
    tft.print((LST - (int)LST)*60, 0);
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
  tft.setTextColor(l_text);
  tft.print(OBJECT_NAME);
  tft.setTextSize(2);
      if ((OBJECT_NAME != "CP") && (IS_BT_MODE_ON == false)){
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
  if ((OBJECT_NAME != "") && (ALT < 0)){
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
  tft.setTextColor(l_text);
  tft.setTextSize(1);
  tft.println(OBJECT_DETAILS);
  tft.println("");
  tft.setTextSize(2);
  tft.print("RA:");
  tft.print(OBJECT_RA_H,0);
  tft.print("h ");
  tft.print(OBJECT_RA_M,2);
  tft.println("'");
  tft.print("DEC:");
  tft.print(OBJECT_DEC_D,0);
  tft.setTextSize(1);
  tft.print("o ");
  tft.setTextSize(2);
  tft.print(abs(OBJECT_DEC_M),2);
  tft.println("'");
// End data for the observed object...
}else{
  tft.setTextSize(2);
  tft.setTextColor(texts);
  tft.println("");
  tft.println("No object selected!");
  tft.setTextSize(1);
  tft.println("Use LOAD button below to select");
  tft.println("object from Messier or NGC Catalogue.");
}

  tft.drawLine(0, 275, 240, 275, btn_d_border);
  tft.drawLine(0, 276, 240, 276, btn_d_border);
  tft.drawLine(0, 277, 240, 277, btn_l_border);

//  Draw Main Screen Menu:
    drawMainScreen_Menu(0);
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
  DrawButton(160,5,70,25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);

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
  tft.print("Coordinates represented here are based on the current motor positions and re-calculated back to Hour Angle, Right Ascension and Declination. NB: Note that alignment affects the scales and small variations are possible!");
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
    DrawButton(1,40,117,30, "Messier", btn_d_border, btn_l_border, btn_l_text, 2);
  }else{
    DrawButton(1,40,117,30, "Messier", 0, btn_l_border, btn_l_text, 2);
  }
  if (LOAD_SELECTOR == 2){
    DrawButton(120,40,119,30, "Treasures", btn_d_border, btn_l_border, btn_l_text, 2);
  }else{
    DrawButton(120,40,119,30, "Treasures", 0, btn_l_border, btn_l_text, 2);
  }

  tft.fillRect(0, 353, 240, 2, btn_d_border);
  tft.drawLine(0, 355, 240, 355, btn_l_border);
 
  DrawButton(160,5,70,25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);
  DrawButton(5,360,80,35, "<prev", btn_d_border, btn_l_border, btn_l_text, 2);
  DrawButton(157,360,80,35, "next>", btn_d_border, btn_l_border, btn_l_text, 2);
  
   if (LOAD_SELECTOR == 1){
   // I'll draw 15 objects per page, thus "(pager*15)" will give me the start of the [index_]
   int kk = MESS_PAGER*15;
   
      for (int i=0; i<5; i++){
          for (int j=0; j<3; j++){
            String M_NAME = Messier_Array[kk].substring(0,Messier_Array[kk].indexOf(';'));
          if (M_NAME == ""){
            break;
          }
          DrawButton(((j*75)+9),((i*50)+85),71,45, M_NAME, messie_btn, 0, btn_l_text, 2);
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
          DrawButton(((j*75)+9),((i*50)+85),71,45, M_NAME, messie_btn, 0, btn_l_text, 1);
          ll += 1;
          }
      }
  }
}

void drawOptionsScreen(){
  CURRENT_SCREEN = 7;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("Options");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(160,5,70,25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);

  // Draw Options...

  // Tracking Mode
  tft.setCursor(10, 45);
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  tft.println("Tracking Modes");
   if (Tracking_type == 0){
      DrawButton(5,65,73,30, "Celest", 0, btn_l_border, btn_l_text, 2);
      DrawButton(83,65,73,30, "Lunar", btn_d_border, btn_l_border, btn_l_text, 2);
      DrawButton(161,65,73,30, "Solar", 0, btn_l_border, btn_l_text, 2);
   }else if (Tracking_type == 1){
      DrawButton(5,65,73,30, "Celest", btn_d_border, btn_l_border, btn_l_text, 2);
      DrawButton(83,65,73,30, "Lunar", 0, btn_l_border, btn_l_text, 2);
      DrawButton(161,65,73,30, "Solar", 0, btn_l_border, btn_l_text, 2);
  }else if (Tracking_type == 2) {
      DrawButton(5,65,73,30, "Celest", 0, btn_l_border, btn_l_text, 2);
      DrawButton(83,65,73,30, "Lunar", 0, btn_l_border, btn_l_text, 2);
      DrawButton(161,65,73,30, "Solar", btn_d_border, btn_l_border, btn_l_text, 2);
  }

  //tft.drawLine (5, 110, 235, 110, btn_l_text);

  // Screen Brightnes
  tft.setCursor(10, 105);
  tft.println("Screen Brightnes");
  DrawButton(200,125,35,30, "MAX", 0, btn_l_border, btn_l_text, 1);
  tft.fillTriangle(5,152,194,152,194,129,btn_d_border);

  // Screen Brightnes
  tft.setCursor(10, 165);
  tft.setTextSize(2);
  tft.print("Screen Auto Off");
//  tft.setCursor(105, 170);
//  tft.setTextSize(1);
//  tft.println(" (in seconds)");
  if (TFT_timeout == 0){
    DrawButton(5,185,40,30, "NEVER", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(5,185,40,30, "NEVER", 0, btn_l_border, btn_l_text, 1);    
  }
  if (TFT_timeout == 30000){
    DrawButton(48,185,35,30, "30s", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(48,185,35,30, "30s", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 60000){
    DrawButton(86,185,35,30, "60s", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(86,185,35,30, "60s", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 120000){
    DrawButton(124,185,35,30, "2m", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(124,185,35,30, "2m", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 300000){
    DrawButton(162,185,35,30, "5m", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(162,185,35,30, "5m", 0, btn_l_border, btn_l_text, 1);
  }
  if (TFT_timeout == 600000){
    DrawButton(200,185,35,30, "10m", btn_d_border, btn_l_border, btn_l_text, 1);
  }else{
    DrawButton(200,185,35,30, "10m", 0, btn_l_border, btn_l_text, 1);
  }
  // Auto Meridian Flip
  tft.setCursor(10, 225);
  tft.setTextSize(2);
  tft.println("Meridian Flip");
  if (IS_MERIDIAN_FLIP_AUTOMATIC){
      DrawButton(5,245,73,30, "AUTO", btn_d_border, btn_l_border, btn_l_text, 2);
      DrawButton(83,245,73,30, "OFF", 0, btn_l_border, btn_l_text, 2);
  }else{
      DrawButton(5,245,73,30, "AUTO", 0, btn_l_border, btn_l_text, 2);
      DrawButton(83,245,73,30, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);    
  }

  // Sound On/Off
  tft.setCursor(10, 285);
  tft.println("Sound On/Off");
  if (IS_SOUND_ON){
      DrawButton(5,305,73,30, "ON", btn_d_border, btn_l_border, btn_l_text, 2);
      DrawButton(83,305,73,30, "OFF", 0, btn_l_border, btn_l_text, 2);
  }else{
      DrawButton(5,305,73,30, "ON", 0, btn_l_border, btn_l_text, 2);
      DrawButton(83,305,73,30, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);    
  }

  // Stepper Motors On/Off
  tft.setCursor(10, 345);
  tft.println("Stepper Motors");
  if (IS_STEPPERS_ON){
      DrawButton(5,365,73,30, "ON", btn_d_border, btn_l_border, btn_l_text, 2);
      DrawButton(83,365,73,30, "OFF", 0, btn_l_border, btn_l_text, 2);
  }else{
      DrawButton(5,365,73,30, "ON", 0, btn_l_border, btn_l_text, 2);
      DrawButton(83,365,73,30, "OFF", btn_d_border, btn_l_border, btn_l_text, 2);    
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
  DrawButton(160,5,70,25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);

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
              String Composed = ObservedObjects[i].substring(0, i1) +" @"+ ObservedObjects[i].substring(i2+1, i3) + " for " + String(tt) + "min | Alt: "+degs_+"deg\n";
              //Composed += "\nAt the time of observation the object was "+degs_+"deg. above horizon, with HA:"+ha_+" Environment wise: " + ObservedObjects[i].substring(i3+1, i4) + " C and " + ObservedObjects[i].substring(i4+1, i5) + "% humidity. ";
              tft.print(Composed);
            }
}

void drawStarMap(){
  CURRENT_SCREEN = 11;
  tft.fillScreen(BLACK);;
  tft.fillRect(1, 1, 239, 35, title_bg);
  tft.drawLine(0, 37, 240, 37, title_bg);
  tft.setCursor(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextSize(3);
  tft.print("StarMap");
  tft.setTextColor(btn_l_text);
  tft.setTextSize(2);
  DrawButton(160,5,70,25, "<back", btn_d_border, btn_l_border, btn_l_text, 2);

  // Draw Star Map...
  String PIC_StarMap = "starmap/";
  if (IS_NIGHTMODE){
    PIC_StarMap += "night/";
  }else{
    PIC_StarMap += "day/";
  }
  // Need to calculate which image to show
  // Images are named as in a matrix 
  // (1,8 1,7 1,6 .... 1,1)
  // (2,8 2,7 2,6 .... 2,1)
  // (3,8 3,7 3,6 .... 3,1)
  // 1,1  = DEC > 30 deg and 3h > RA > 0h;
  // 1,2  = DEC > 30 deg and 6h> RA > 3h;
  // 2,1 = -30 deg > DEC > 30 deg and 3h > RA > 0h;
  // ......

  Current_RA_DEC();
  float DEC_dd = curr_DEC_D + curr_DEC_M/60;
  int telescope_X = 0;
  int telescope_Y = 0;

  int tmp_map_r = 0;
  int tmp_map_c = 0;
        if (DEC_dd > 30){
          tmp_map_r = 1;
        }else if (DEC_dd < -30){
          tmp_map_r = 3;
        }else{
          tmp_map_r = 2;
        }
        tmp_map_c = int((curr_RA_H+curr_RA_M/60)/3)+1;

 if ((tmp_map_c == map_c) && (tmp_map_r == map_r)){
    IS_CUSTOM_MAP_SELECTED = false;
 }
 
 // First time only after you Start the rDUINOScope map, it needs this routine!
 if ((map_c == 0) && (map_r == 0)){
   map_c = tmp_map_c;
   map_r = tmp_map_r;
 }

   if (!IS_CUSTOM_MAP_SELECTED){
       map_c = tmp_map_c;
       map_r = tmp_map_r;
      // Now decide where is the Telescope Pointing...
      // Only calculate Telescope IF: -70 < DEC < 70 deg.
      if ((DEC_dd < 70) && (DEC_dd > -70)){
          telescope_X = 240 - (80*((curr_RA_H + curr_RA_M/60) - (tmp_map_c - 1)*3));
          if (tmp_map_r == 2){
            if ((DEC_dd > -20) && (DEC_dd < 20)){
               telescope_Y = 220 + DEC_dd * -5.35;
            }else if (DEC_dd > 20){
               telescope_Y = 113 - (DEC_dd - 20) * 7.1;
            }else if (DEC_dd < -20){
               telescope_Y = 327 + (abs(DEC_dd) - 20) * 7.1;          
            }
          }else{
            if ((abs(DEC_dd) > 30) && (abs(DEC_dd) < 40)){
              telescope_Y = 400 - (DEC_dd - 30) * 5.2;
            }else if ((abs(DEC_dd) > 40) && (abs(DEC_dd) < 60)){
               telescope_Y = 347 - (DEC_dd - 40) * 8.4;
            }else if (DEC_dd > 60){
               telescope_Y = 177 - (DEC_dd - 60) * 13.7;          
            }  
            if (DEC_dd < 0){ 
              telescope_Y = 400 - telescope_Y;
            }
          }
      }
  }
  PIC_StarMap += String(map_r) + "-" + String(map_c) + ".bmp";
  StarMaps = SD.open(PIC_StarMap);
  drawPic(&StarMaps, 0, 40, 240, 360);
  StarMaps.close();
  if (!IS_CUSTOM_MAP_SELECTED){
    tft.drawCircle(telescope_X, telescope_Y, 7, RED);
    tft.drawLine(0, telescope_Y, 240, telescope_Y, RED);
    tft.drawLine(telescope_X, 40, telescope_X, 400, RED);
  }
  IS_CUSTOM_MAP_SELECTED = false;
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

  DrawButton(160,5,70,25, "DONE>", btn_d_border, btn_l_border, btn_l_text, 2);

  tft.fillRect(0, 353, 240, 2, btn_d_border);
  tft.drawLine(0, 355, 240, 355, btn_l_border);
  
   // I'll draw 15 objects per page, thus "(pager*15)" will give me the start of the [index_]
   DrawButton(5,360,80,35, "<prev", btn_d_border, btn_l_border, btn_l_text, 2);
   DrawButton(157,360,80,35, "next>", btn_d_border, btn_l_border, btn_l_text, 2);

   if (ALLIGN_TYPE == 3){
       int kk = STARS_PAGER*15;
       for (int i=0; i<5; i++){
          for (int j=0; j<3; j++){
              int i1 = Iter_Stars[kk].indexOf(';');
              int i2 = Iter_Stars[kk].indexOf(';',i1+1);
              String S_NAME = Iter_Stars[kk].substring(i1+1, i2);
              String C_NAME = Iter_Stars[kk].substring(0, i1);
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
   }else{
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

  if (ALLIGN_TYPE != 3){
      DrawButton(5,360,100,35, "<Repeat", 0, btn_l_border, btn_l_text, 2);
  }else{
      DrawButton(5,360,100,35, "<Exit", 0, btn_l_border, btn_l_text, 2);
  }
  if (ALLIGN_STEP == 1){
      DrawButton(127,360,110,35, "SYNC !", 0, btn_l_border, btn_l_text, 2); 
      tft.setCursor(0, 50);
      tft.println("- Use manual motor movement to center the star!");
      tft.println("");
      tft.println("- Once centered, hit 'SYNC !' button.");  
      if (ALLIGN_TYPE == 3){
          tft.println("");
          tft.print("NB! Selected Object ->");   
          tft.println(OBJECT_NAME);     
      }  
  }else if (ALLIGN_STEP == 2){
      if (ALLIGN_TYPE == 3){
          DrawButton(127,360,110,35, "NEXT", 0, btn_l_border, btn_l_text, 2); 
          tft.setCursor(0,50);
          tft.println("- Use Alt/Az (phisical) knobs on your mount to center on Polaris!");
          tft.println("");
          tft.println("- Once centered, hit 'ALIGN' button.");        
          tft.println("");
          tft.print("NB! Only correct halfway to center! ->");   
          tft.println(OBJECT_NAME);     
      }else{
          DrawButton(127,360,110,35, "CENTER", 0, btn_l_border, btn_l_text, 2); 
          tft.setCursor(0,50);
          tft.println("- Use manual motor movement to center the star!");
          tft.println("");
          tft.println("- Once centered, hit 'SYNC !' button.");  
      }
  }
}


void OnScreenMsg(int Msg){
// Msg = 1 -> Moving;
// Msg = 2 -> Tracking Off;
// Msg = 3 -> Select Different Star;
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
    tft.setCursor(58, 165);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(48, 205);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(58, 225);
    tft.print(m3);
    
  }else if (Msg == 3){
    m1 = "ERROR!";
    m2 = "Not Visible";
    m3 = "";
    //Timer3.stop(); // 
    //IS_TRACKING = false;
    tft.setCursor(64, 165);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(48, 205);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(58, 225);
    tft.print(m3);
    
  }else if (Msg == 4){
    m1 = "PICK AGAIN";
    m2 = "ALT=" + String(ALT,0);
    m3 = "RA= " + String(OBJECT_RA_H,0);
    //Timer3.stop(); // 
    //IS_TRACKING = false;
    tft.setCursor(50, 165);
    tft.setTextSize(3);
    tft.println(m1);
    tft.setCursor(50, 205);
    tft.setTextSize(2);
    tft.print(m2);
    tft.setCursor(50, 225);
    tft.print(m3);
  }
}

void considerDayNightMode(){
  boolean prev_night_mode = IS_NIGHTMODE;
          if (analogRead(A6) < 800){
              IS_NIGHTMODE = true;
          } else {
              IS_NIGHTMODE = false;
          }
      // Now make sure it works for all screen - redraws them when mode switched  
      if (prev_night_mode != IS_NIGHTMODE){
         if (IS_NIGHTMODE == true){   // Night Mode
            texts = Maroon;
            l_text = RED;
            d_text = Maroon;
            title_bg = RED;
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

           if (CURRENT_SCREEN == 0){
                drawGPSScreen();
           }else if (CURRENT_SCREEN == 1){
                drawClockScreen();
           }else if (CURRENT_SCREEN == 3){
                drawSelectAlignment();
           }else if (CURRENT_SCREEN == 4){
                drawMainScreen();
           }else if (CURRENT_SCREEN == 5){
                drawCoordinatesScreen();
           }else if (CURRENT_SCREEN == 6){
                drawLoadScreen();
           }else if (CURRENT_SCREEN == 7){
                drawOptionsScreen();
           }else if (CURRENT_SCREEN == 10){
                drawSTATScreen();
           }else if (CURRENT_SCREEN == 12){
                drawStarSyncScreen();
           }else if (CURRENT_SCREEN == 13){
                drawConstelationScreen(0);
           }
     }
 }


void drawMainScreen_Menu(int fill){
//  if (fill == 1){
//    tft.fillRect(0, 278, 239, 122, BLACK);
//  }

  if (MAIN_SCREEN_MENU == 0){
     if (IS_BT_MODE_ON == false){
          DrawButton( 1, 280, 78, 55, "LOAD", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 81, 280, 78, 55, "HOME", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 1, 339, 78, 55, "STATs", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 81, 339, 78, 55, "+", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 161, 339, 78, 55, "BT", 0, btn_l_border, btn_l_text, 2);
      } else{
          DrawButton( 1, 280, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 81, 280, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 1, 339, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 81, 339, 78, 55, "+", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 161, 339, 78, 55, "BT", btn_d_border, btn_l_border, btn_l_text, 2);
      }
      if (IS_TRACKING == true){
        DrawButton( 161, 280, 78, 55, "TRACK", btn_d_border, btn_l_border, btn_l_text, 2);
      }else{
        DrawButton( 161, 280, 78, 55, "TRACK", 0, btn_l_border, btn_l_text, 2);
      }
      MAIN_SCREEN_MENU = 0;
  }else{
     if (IS_BT_MODE_ON == false){
          DrawButton( 1, 280, 78, 55, "COORD", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 81, 280, 78, 55, "MAP", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 1, 339, 78, 55, "OPTION", 0, btn_l_border, btn_l_text, 2);
          DrawButton( 81, 339, 78, 55, "+", 0, btn_l_border, btn_l_text, 2);
      } else{
          DrawButton( 1, 280, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 81, 280, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 1, 339, 78, 55, "", 0, btn_d_border, btn_l_text, 2);
          DrawButton( 81, 339, 78, 55, "+", 0, btn_l_border, btn_l_text, 2);
      }
      if (IS_DEV1_ON){
          DrawButton( 161, 280, 78, 55, "DEV 1", btn_d_border, btn_l_border, btn_l_text, 2);
      }else{
          DrawButton( 161, 280, 78, 55, "DEV 1", 0, btn_l_border, btn_l_text, 2);
      }
      if (IS_DEV2_ON){
          DrawButton( 161, 339, 78, 55, "DEV 2", btn_d_border, btn_l_border, btn_l_text, 2);
      }else{
          DrawButton( 161, 339, 78, 55, "DEV 2", 0, btn_l_border, btn_l_text, 2);
      }
      MAIN_SCREEN_MENU = 1;
  }
}

void TimerUpdateDraw(int z){
   tft.setTextColor(d_text);
   w_DateTime[dateEntryPos] = z;
   if (dateEntryPos >=0 && dateEntryPos <2){
      tft.fillRect((dateEntryPos*18)+30,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+30,55);
   }else if (dateEntryPos >1 && dateEntryPos <4){
      tft.fillRect((dateEntryPos*18)+48,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+48,55);
   }else if (dateEntryPos >3 && dateEntryPos <8){
      tft.fillRect((dateEntryPos*18)+66,55,18,22,BLACK);
      tft.setCursor((dateEntryPos*18)+66,55);
   }else if (dateEntryPos >7 && dateEntryPos <10){
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

