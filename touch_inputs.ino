//
//    rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    PROJECT Website: http://rduinoscope.co.nf/
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
//  - Handles ALL TOUCH SCREEN Inputs;
//  - Screens are separated like:
//    * CURRENT_SCREEN==0  - drawGPSScreen() Where the GPS coordinates are displayed
//    * CURRENT_SCREEN==1  - drawClockScreen() Captures updates on the time and date
//    * CURRENT_SCREEN==2  -                - not used
//    * CURRENT_SCREEN==3  - Select Alignment method (only have 3 buttons)
//    * CURRENT_SCREEN==4  - drawMainScreen() Captures all clicks on the MAIN Screen of the application
//    * CURRENT_SCREEN==5  - drawCoordinatesScreen() Only have "back" button
//    * CURRENT_SCREEN==6  - drawLoadScreen() Captures input on Load screen (all of them: Messier && Treasurres)
//    * CURRENT_SCREEN==7  -                - not used
//    * CURRENT_SCREEN==8  -                - not used
//    * CURRENT_SCREEN==9  -                - not used
//    * CURRENT_SCREEN==10  - drawSTATScreen()
//    * CURRENT_SCREEN==11  -               - not used
//    * CURRENT_SCREEN==12  - drawStarSyncScreen() - To Select Alignment Star; 
//    * CURRENT_SCREEN==13  - drawConstelationScreen(int indx) - to actually align on Star. Called few times per alignment procedure.
//
//  - Please note, that Touches are separated in 2 section to capture OnPress && OnRelease!
//    You will notice that "if (lx > 0 && ly > 0 )" this part defines OnPress activities.

void considerTouchInput(int lx, int ly){
  //**************************************************************
  //
  //      BUTTON DOWN Events start here
  //
  //      - only executed when the user touches the screen - PRESS
  //**************************************************************
  if (lx > 0 && ly > 0 ){
    if (CURRENT_SCREEN == 0){   // captures touches on drawGPSScreen()
      if (lx > 46 && lx < 192 && ly > 317 && ly < 371){
        last_button = 1;
        tft.fillRect(45,316,148,58, btn_l_border);
      }
    }else if (CURRENT_SCREEN == 1){   // captures touches on drawClockScreen()
       if (lx > 160 && lx < 230 && ly > 336 && ly < 393){
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
           _Stemp = dht.readTemperature();;
           _Shumid = dht.readHumidity();
           
           drawSelectAlignment();
           //drawStarSyncScreen();
           //drawMainScreen();
       }else if (lx > 11 && lx < 83 && ly > 156 && ly < 213){
         // BTN 1 pressed
          tft.drawRect(10,155,73,58, btn_l_border);
          last_button = 1;
          removeTime_addXX();
       }else if (lx > 86 && lx < 158 && ly > 156 && ly < 213){
         // BTN 2 pressed
          tft.drawRect(85,155,73,58, btn_l_border);
          last_button = 2;
          removeTime_addXX();
       }else if (lx > 161 && lx < 233 && ly > 156 && ly < 213){
         // BTN 3 pressed
          tft.drawRect(160,155,73,58, btn_l_border);
          last_button = 3;
          removeTime_addXX();
       }else if (lx > 11 && lx < 83 && ly > 216 && ly < 273){
         // BTN 4 pressed
          tft.drawRect(10,215,73,58, btn_l_border);
          last_button = 4;
          removeTime_addXX();
       }else if (lx > 86 && lx < 158 && ly > 216 && ly < 273){
         // BTN 5 pressed
          tft.drawRect(85,215,73,58, btn_l_border);
          last_button = 5;
          removeTime_addXX();
       }else if (lx > 161 && lx < 233 && ly > 216 && ly < 273){
         // BTN 6 pressed
          tft.drawRect(160,215,73,58, btn_l_border);
          last_button = 6;
          removeTime_addXX();
       }else if (lx > 11 && lx < 83 && ly > 276 && ly < 333){
         // BTN 7 pressed
          tft.drawRect(10,275,73,58, btn_l_border);
          last_button = 7;
          removeTime_addXX();
       }else if (lx > 86 && lx < 158 && ly > 276 && ly < 333){
         // BTN 8 pressed
          tft.drawRect(85,275,73,58, btn_l_border);
          last_button = 8;
          removeTime_addXX();
       }else if (lx > 161 && lx < 233 && ly > 276 && ly < 333){
         // BTN 9 pressed
          tft.drawRect(160,275,73,58, btn_l_border);
          last_button = 9;
          removeTime_addXX();
       }else if (lx > 86 && lx < 158 && ly > 336 && ly < 393){
         // BTN 0 pressed
          tft.drawRect(85,335,73,58, btn_l_border);
          last_button = 10;
          removeTime_addXX();
       } else if (lx > 11 && lx < 83 && ly > 336 && ly < 393){
       // BTN SummerTime pressed
         last_button = 22;
      }
    }else if (CURRENT_SCREEN == 3){
      if (lx > 20 && lx < 200 && ly > 100 && ly < 155){
         // BTN "1 Star Alignment" pressed
          DrawButton(20,100,200,55, "1 Star Alignment", btn_l_border, 0, btn_l_text, 2);
          ALLIGN_TYPE = 1;
          drawStarSyncScreen();
       }else if (lx > 20 && lx < 200 && ly > 170 && ly < 225){
         // BTN "Iterative Alignment" pressed
          DrawButton(20,170,200,55, "Iterative Align", btn_l_border, 0, btn_l_text, 2);
          ALLIGN_TYPE = 3;
          
          // I'll take some time to Initialize the Sub ARRAY with suitable stars (only for Northern Hemi)
          tft.setCursor(0, 5);
          int cc = 0;
          for (int t=0; t < 203; t++){
              int i1 = Stars[t].indexOf(';');
              int i2 = Stars[t].indexOf(';',i1+1);
              int i3 = Stars[t].indexOf(';',i2+1);
              OBJECT_NAME = Stars[t].substring(i1+1,i2)+" from "+Stars[t].substring(0,i1);
              String OBJ_RA = Stars[t].substring(i2+1,i3);
              OBJECT_RA_H = OBJ_RA.substring(0,OBJ_RA.indexOf('h')).toFloat();
              OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h')+1,OBJ_RA.length()-1).toFloat();
              float ts_RA = OBJECT_RA_H + OBJECT_RA_M/100;
              String OBJ_DEC = Stars[t].substring(i3,Stars[t].length());
              String sign = OBJ_DEC.substring(0, 1);
              OBJECT_DEC_D = OBJ_DEC.substring(1,OBJ_DEC.indexOf('°')).toFloat();
              if (sign == "-"){ 
                  OBJECT_DEC_D *= (-1);
              }
              OBJECT_DEC_M = 0;
              if (ts_RA > 5.3 && ts_RA < 23.3){
                  calculateLST_HA();
                  if (AZ > 90 && AZ < 270 && ALT > 20){
                       Iter_Stars[cc] = Stars[t];
                       cc += 1;
                       if (cc > 49){
                            break;
                       }
                  }
              }
              int_star_count = floor(cc/15) + 1;    
          }
          drawStarSyncScreen();
       }else if (lx > 20 && lx < 200 && ly > 320 && ly < 365){
         // BTN "Skip Alignment" pressed
          DrawButton(20,320,200,45, "Skip Alignment", btn_l_border, 0, btn_l_text, 2);
          IS_IN_OPERATION = true;
          drawMainScreen();
       }
    }else if (CURRENT_SCREEN == 4){     // captures touches on drawMainScreen()
      if (lx > 0 && lx < 55 && ly > 0 && ly < 55){
       // Load the GPS screen to capture new data && correct time if needed on the next screen (Time Screen)
         tft.drawRect(0,0,55,55, btn_l_border);
         CURRENT_SCREEN =0;
         GPS_iterrations = 0;
         IS_IN_OPERATION = false;
         ALLIGN_STEP = 0;
         ALLIGN_TYPE = 0; 
         drawGPSScreen();
      }
      if (lx > 2 && lx < 78 && ly > 282 && ly < 335 && IS_BT_MODE_ON == false){
       // BTN 1 pressed
         tft.fillRect(1,280,78,55, btn_l_border);
         last_button = 1;
      } else if (lx > 2 && lx < 78 && ly > 341 && ly < 400 && IS_BT_MODE_ON == false){
       // BTN 4 pressed
         tft.fillRect(1,339,78,55, btn_l_border);
         last_button = 4;
      } else if (lx > 83 && lx < 148 && ly > 280 && ly < 335 && IS_BT_MODE_ON == false){
       // BTN 2 pressed
         tft.fillRect(81,280,78,55, btn_l_border);
         last_button = 2;
      } else if (lx > 83 && lx < 148 && ly > 341 && ly < 400 && IS_BT_MODE_ON == false){
       // BTN 5 pressed
         tft.fillRect(81,339,78,55, btn_l_border);
         last_button = 5;
      } else if (lx > 163 && lx < 239 && ly > 282 && ly < 335){
       // BTN 3 pressed
         tft.fillRect(161,280,78,55, btn_l_border);
         last_button = 3;
      } else if (lx > 163 && lx < 239 && ly > 341 && ly < 400){
       // BTN 6 pressed
         tft.fillRect(161,339,78,55, btn_l_border);
         last_button = 6;
      }
    }else if (CURRENT_SCREEN == 5){   // captures touches on drawCoordinatesScreen()
       if (lx > 181 && lx < 238 && ly > 5 && ly < 35){
       // BTN Cancel pressed
         drawMainScreen();
       }
    }else if (CURRENT_SCREEN == 6){   // captures touches on drawLoadScreen() .. the one that loads objects from DB
       if (lx > 181 && lx < 238 && ly > 5 && ly < 35){
       // BTN Cancel pressed
         drawMainScreen();
       }
       if (lx > 157 && lx < 238 && ly > 360 && ly < 400){
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
       if (lx > 5 && lx < 85 && ly > 360 && ly < 400){
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
       if (lx > 1 && lx < 117 && ly > 40 && ly < 70){
       // BTN Messier pressed
           LOAD_SELECTOR = 1;
           drawLoadScreen();
       }
       if (lx > 120 && lx < 240 && ly > 40 && ly < 70){
       // BTN File pressed
           LOAD_SELECTOR = 2;
           drawLoadScreen();
       }       
       if (LOAD_SELECTOR == 1){       
         // I'm in MESSIER selector and need to check which Messier object is pressed
         for (int i=0; i<5; i++){
            for (int j=0; j<3; j++){
              if (lx > ((j*75)+10) && lx < ((j*75)+79) && ly > ((i*50)+86) && ly < ((i*50)+129)){
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
              if (lx > ((j*75)+10) && lx < ((j*75)+79) && ly > ((i*50)+86) && ly < ((i*50)+129)){
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
     }else if (CURRENT_SCREEN == 10){   // captures touches on drawSTATScreen()
       if (lx > 181 && lx < 238 && ly > 5 && ly < 35){
       // BTN Cancel pressed
         IS_IN_OPERATION = true;
         drawMainScreen();
        }
    }else if (CURRENT_SCREEN == 12){    // captures touches on drawStarSyncScreen()
       if (lx > 181 && lx < 238 && ly > 5 && ly < 35){
       // BTN SKIP pressed
         IS_IN_OPERATION = true;
         drawMainScreen();
       }
       int do_kolko = 0;
       if (ALLIGN_TYPE == 3){
        // Chage the 4 to represent the real count of screens.
        // They need to be dynamically calculated... not fixed     
          do_kolko = int_star_count;
       }else{
          do_kolko = 14;
       }
       if (lx > 157 && lx < 238 && ly > 360 && ly < 400){
       // BTN next> pressed 
             STARS_PAGER += 1;
             if (STARS_PAGER < do_kolko){
                drawStarSyncScreen();
             }else{
               STARS_PAGER = do_kolko - 1;
             }
       }
       if (lx > 5 && lx < 85 && ly > 360 && ly < 400){
       // BTN <prev pressed
             STARS_PAGER -= 1;
             if (STARS_PAGER >= 0){
                drawStarSyncScreen();
             }else{
                STARS_PAGER = 0;
             }
        }

       if (ALLIGN_TYPE < 3){
           // I'm in STARS selector and need to check which Star object is pressed
           for (int i=0; i<5; i++){
              for (int j=0; j<3; j++){
                if (lx > ((j*75)+10) && lx < ((j*75)+79) && ly > ((i*50)+86) && ly < ((i*50)+129)){
                   // found button pressed.... now I need to get his ID and link to the ARRAY;
                  int zz = (STARS_PAGER*15) + (i*3) + j;
                  if (Stars[zz] != ""){
                    selectOBJECT_M(zz,2);
                    calculateLST_HA();
                    if (ALT < 0){
                        OnScreenMsg(3);
                        SoundOn(note_C,96);
                        delay(2000);
                        drawStarSyncScreen();
                    }else{
                        OnScreenMsg(1);
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
                        ALLIGN_STEP += 1;
                        //drawConstelationScreen(zz);
                    }
                  }
                }
              }
           }
       }else{
           // I'm in STARS selector and need to check which Star object is pressed
           for (int i=0; i<5; i++){
              for (int j=0; j<3; j++){
                if (lx > ((j*75)+10) && lx < ((j*75)+79) && ly > ((i*50)+86) && ly < ((i*50)+129)){
                   // found button pressed.... now I need to get his ID and link to the ARRAY;
                  int zz = (STARS_PAGER*15) + (i*3) + j;
                  if (Iter_Stars[zz] != ""){
                    // selectOBJECT_M(zz,3);
                    Iterative_Star_Index = zz;
                    
                    // replaced the above with the below row...
                    // to make sure each press selects the same star (inital or on Step 2)
                    // change made after Test on 17th March.
                    selectOBJECT_M(Iterative_Star_Index,3);
                    calculateLST_HA();
                    OnScreenMsg(1);
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
                    ALLIGN_STEP += 1;
                    // drawConstelationScreen(zz);
                  }
                }
              }
           }             
       }
    }else if (CURRENT_SCREEN==13){    // captures touches on drawConstelationScreen(int indx)
       if (lx > 5 && lx < 105 && ly > 360 && ly < 400){
          // BTN "<Repeat" or "<EXIT" pressed
          if (ALLIGN_TYPE == 3){
              // delta_a_RA = 0;
              // delta_a_DEC = 0;
              STARS_PAGER = 0;
              IS_TRACKING = false;
              IS_IN_OPERATION = true;
              drawMainScreen();
          }else{
              ALLIGN_STEP -= 1;
              drawStarSyncScreen();
          }
       }
       if (lx > 128 && lx < 240 && ly > 360 && ly < 400){
          // BTN "ALIGN!" pressed
          // Here we need to know which Star is this - 1st, 2nd, 3rd... etc ?
          // In order to use Ralph Pass alignment procedure described on http://rppass.com/  
          // http://rppass.com/align.pdf - the actual PDF
          
          if (ALLIGN_STEP == 1){
              if (ALLIGN_TYPE == 1){
                IS_TRACKING = false;
                IS_IN_OPERATION = true;
                drawMainScreen();
              }else if (ALLIGN_TYPE == 3){
                // Select Polaris and SlewTo...                
                IS_TRACKING = false;                
                selectOBJECT_M(192,2);  // Polaris in on Index 192 in the Stars Array
                calculateLST_HA();
                OnScreenMsg(1);
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
                //drawConstelationScreen(0);
                ALLIGN_STEP = 2;
              }
          }else if (ALLIGN_STEP == 2){
              if(ALLIGN_TYPE == 3){
                // Select First Star Again and SlewTo...                
                IS_TRACKING = false;
                selectOBJECT_M(Iterative_Star_Index,3);  // Load First Star from the Stars Array
                calculateLST_HA();
                    OnScreenMsg(1);
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
                //drawConstelationScreen(0);
                ALLIGN_STEP = 1;
              }
          }
      }
    }
  }else{
  //**************************************************************
  //
  //      BUTTON UP Events start here
  //
  //      - only executed when the user touches the screen - RELEASE
  //**************************************************************
    
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
          drawClockScreen();
      }
    }else if (CURRENT_SCREEN == 1){
        if (last_button == 1){
         last_button = 0;
         tft.drawRect(10,155,73,58, BLACK);
         TimerUpdateDraw(1);
        }
        if (last_button == 2){
         last_button = 0;
         tft.drawRect(85,155,73,58, BLACK);
         TimerUpdateDraw(2);
        }
        if (last_button == 3){
         last_button = 0;
         tft.drawRect(160,155,73,58, BLACK);
         TimerUpdateDraw(3);
        }
        if (last_button == 4){
         last_button = 0;
         tft.drawRect(10,215,73,58, BLACK);
         TimerUpdateDraw(4);
        }
        if (last_button == 5){
         last_button = 0;
         tft.drawRect(85,215,73,58, BLACK);
         TimerUpdateDraw(5);
        }
        if (last_button == 6){
         last_button = 0;
         tft.drawRect(160,215,73,58, BLACK);
         TimerUpdateDraw(6);
        }
        if (last_button == 7){
         last_button = 0;
         tft.drawRect(10,275,73,58, BLACK);
         TimerUpdateDraw(7);
        }
        if (last_button == 8){
         last_button = 0;
         tft.drawRect(85,275,73,58, BLACK);
         TimerUpdateDraw(8);
        }
        if (last_button == 9){
         last_button = 0;
         tft.drawRect(160,275,73,58, BLACK);
         TimerUpdateDraw(9);
        }
        if (last_button == 10){
         last_button = 0;
         tft.drawRect(85,335,73,58, BLACK);
         TimerUpdateDraw(0);
        }
        if (last_button == 22){
            if (Summer_Time == 1){
                Summer_Time = 0;
                tft.fillRect(11, 336, 71, 56,BLACK);
                DrawButton(11,336,71,56, "SUMMER", 0, btn_l_border, btn_l_text, 1);
            }else{
                Summer_Time = 1;
                DrawButton(11,336,71,56, "SUMMER", btn_d_border, btn_l_border, btn_l_text, 1);
            }
            tft.setTextColor(btn_l_text);
            tft.setTextSize(3);
            last_button = 0;
        }
    }else if (CURRENT_SCREEN == 4){
        tft.setTextColor(btn_l_text);

        if (last_button == 1 && IS_BT_MODE_ON == false){
         last_button = 0;
         DrawButton( 1, 280, 78, 55, "LOAD", btn_l_text, 0, btn_d_text, 2);
         drawLoadScreen();
        }
        if (last_button == 4 && IS_BT_MODE_ON == false){
         last_button = 0;
         DrawButton( 1, 339, 78, 55, "COORD", btn_l_text, 0, btn_d_text, 2);
         if (IS_TRACKING == true){
            SoundOn(note_C,32);
         }
         drawCoordinatesScreen();
        }
        if (last_button == 2 && IS_BT_MODE_ON == false){
         last_button = 0;
         DrawButton( 81, 280, 78, 55, "HOME", btn_l_text, 0, btn_d_text, 2);
         IS_TRACKING = false;
         OBJECT_NAME = "CP";
         OBJECT_DESCR = "Celestial pole";
         OBJECT_RA_H = 12;
         OBJECT_RA_M = 0;       
         OBJECT_DEC_D = 90;
         OBJECT_DEC_M = 0;
         delay(1000);
         IS_OBJ_FOUND = false;
         IS_OBJECT_RA_FOUND = false;
         IS_OBJECT_DEC_FOUND = false;
         Slew_timer = millis();
         Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
         OBJECT_DETAILS="The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere";
        }
        if (last_button == 5 && IS_BT_MODE_ON == false){
         last_button = 0;
         DrawButton( 81, 399, 78, 55, "...", btn_l_text, 0, btn_d_text, 2);
         drawSTATScreen();
        }
        if (last_button == 3){
         last_button = 0;
         if (IS_TRACKING == false){
           IS_TRACKING = true;
           DrawButton( 161, 280, 78, 55, "TRACK", btn_d_border, btn_l_border, btn_l_text, 2);
           setmStepsMode("R",MICROSteps);
           Timer3.start(Clock_Motor); 
         }else{
           IS_TRACKING = false;
           tft.fillRect(161, 280, 78, 55,BLACK);
           DrawButton( 161, 280, 78, 55, "TRACK", 0, btn_l_border, btn_l_text, 2);
           setmStepsMode("R",1);
           Timer3.stop(); // 
         }       
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
