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
//  - Bluetooth communication (both ways);
//
void considerBTCommands(){
      if ((BT_COMMAND_STR != ":GR") && (BT_COMMAND_STR != ":GD") && (BT_COMMAND_STR != "")){
        if (IS_SOUND_ON){
          SoundOn(note_c,8);
        }
        BTs += "\r\n";
        BTs += rtc.getTimeStr(FORMAT_LONG);
        BTs += ": ";
        BTs += BT_COMMAND_STR;
      }

      if (BT_COMMAND_STR.indexOf("synClock")>0){
        // :synClock HH:MM:SS:YYYY:M:D:SummerTime#
        int i = BT_COMMAND_STR.indexOf("synClock");
        String date_time = BT_COMMAND_STR.substring(i+9,BT_COMMAND_STR.length());
        int k0 = date_time.indexOf(":");
        int k1 = date_time.indexOf(":", k0+1);
        int k2 = date_time.indexOf(":", k1+1);
        int hh = date_time.substring(0,k0).toInt();
        int mm = date_time.substring(k0+1,k1).toInt();
        int ss = date_time.substring(k1+1,k2).toInt()+1;
        rtc.setTime(hh,mm,ss);
        int k3 = date_time.indexOf(":", k2+1);
        int k4 = date_time.indexOf(":", k3+1);
        int k5 = date_time.indexOf(":", k4+1);
        int yy = date_time.substring(k2+1,k3).toInt();
        int mo = date_time.substring(k3+1,k4).toInt();
        int dd = date_time.substring(k4+1,k5).toInt();
        Summer_Time = date_time.substring(k5+1,date_time.length()).toInt();
        rtc.setDate(dd,mo,yy);

        Serial3.print("synClock: ");
        Serial3.print(rtc.getTimeStr(FORMAT_LONG));
        Serial3.print(" ");
        Serial3.print(rtc.getDateStr(FORMAT_LITTLEENDIAN));
        Serial3.print(" summer_time=");
        Serial3.print(Summer_Time);
        Serial3.println(" .....(OK)");

        BT_COMMAND_STR = "";
      }
      if (BT_COMMAND_STR.indexOf("synGPS")>0){
        // :synGPS LAT:LONG:ALT:TZ# (TZ = time zone)
        int i = BT_COMMAND_STR.indexOf("synGPS");
        String gps_str = BT_COMMAND_STR.substring(i+7,BT_COMMAND_STR.length());
        int k0 = gps_str.indexOf(":");
        int k1 = gps_str.indexOf(":", k0+1);
        int k2 = gps_str.indexOf(":", k1+1);
        OBSERVATION_LATTITUDE = gps_str.substring(0,k0).toFloat();
        OBSERVATION_LONGITUDE = gps_str.substring(k0+1,k1).toFloat();
        OBSERVATION_ALTITUDE = gps_str.substring(k1+1,k2).toFloat();
        TIME_ZONE = gps_str.substring(k2+1,gps_str.length()).toInt();
        Serial3.print("synGPS: ");
        Serial3.print(OBSERVATION_LATTITUDE, 6);
        Serial3.print(" | ");
        Serial3.print(OBSERVATION_LONGITUDE, 6);
        Serial3.print(" | ");
        Serial3.print(OBSERVATION_ALTITUDE, 2);
        Serial3.println(" .....(OK)");
        BT_COMMAND_STR = "";
      }
      if (BT_COMMAND_STR.indexOf("gotoHome")>0){
         IS_TRACKING = false;
         Timer3.stop();
         OBJECT_NAME = "CP";
         OBJECT_DESCR = "Celestial pole";
         OBJECT_RA_H = 12;
         OBJECT_RA_M = 0;       
         OBJECT_DEC_D = 90;
         OBJECT_DEC_M = 0;
         IS_OBJ_FOUND = false;
         IS_OBJECT_RA_FOUND = false;
         IS_OBJECT_DEC_FOUND = false;
         Slew_timer = millis();
         Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
         OBJECT_DETAILS="The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere";
      }
      if (BT_COMMAND_STR=="RD_priv"){
        Current_RA_DEC();
        Serial3.print("Right Ascension: ");
        Serial3.println(curr_RA_lz);
        Serial3.print("Declination: ");
        Serial3.print(curr_DEC_lz);
        BT_COMMAND_STR = "";
      }
      // :AP# - Start Tracking
      if (BT_COMMAND_STR == ":AP"){
        if (IS_TRACKING == false){
          IS_TRACKING = true;
          if (Tracking_type == 1){  // 1: Sidereal, 2: Solar, 0: Lunar;
              Timer3.start(Clock_Sidereal);
          }else if (Tracking_type == 2){
              Timer3.start(Clock_Solar);
          }else if (Tracking_type == 0){
              Timer3.start(Clock_Lunar);            
          }
          drawMainScreen();
        }
        BT_COMMAND_STR = "";
      }  
      // :AL#  - Stop Tracking    
      if (BT_COMMAND_STR==":AL"){
        if (IS_TRACKING == true){
          IS_TRACKING = false;
          Timer3.stop();
          drawMainScreen();
        }
        BT_COMMAND_STR = "";
      }       
      // :CM# or :CMR# - Sync with selected Object    
      if ((BT_COMMAND_STR==":CM")||(BT_COMMAND_STR==":CMR")){
       float HAHh;
       float HAMm;
       float HA_deci;
       
       if (HAHour >= 12){
          HAHh = HAHour - 12;
       }else{
          HAHh = HAHour;
       }

       HAMm = HAMin;
       HA_deci = (HAHh+(HAMm/60))*15;   // In degrees - decimal

       delta_a_RA = (double(RA_microSteps) - double(HA_deci * HA_H_CONST))/double(HA_H_CONST);
       delta_a_DEC = (double(DEC_microSteps) - double(SLEW_DEC_microsteps))/double(DEC_D_CONST);

        Serial3.print("Coordinates  matched #");
        BT_COMMAND_STR = "";
      }
      // :GR#  - Request RA coordinate    
      if (BT_COMMAND_STR==":GR"){
        Current_RA_DEC();
        Serial3.print(curr_RA_lz);
        Serial3.print("#");
        BT_COMMAND_STR = "";
      }
      // :GD#  - Request DEC coordinate    
      if (BT_COMMAND_STR==":GD"){
        Current_RA_DEC();
        Serial3.print(curr_DEC_lz);
        Serial3.print("#");
        BT_COMMAND_STR = "";
      }
     // :Sr 07:08:52# - Set Target RA
      if (BT_COMMAND_STR.indexOf("Sr")>0){
        // LX200 - RA Coordinates ()
        
        int i = BT_COMMAND_STR.indexOf("Sr");
        String _RA = BT_COMMAND_STR.substring(i+2,BT_COMMAND_STR.length()-1);
        int k0 = _RA.indexOf(":");
        int k1 = _RA.indexOf(":", k0+1);
        OBJECT_DETAILS = "_RA:" + _RA;
        OBJECT_RA_H = _RA.substring(0,k0).toFloat();
        float RAse = 0;
        if (_RA.substring(k1 + 1,_RA.length()).length()<2){
          RAse = _RA.substring(k1 + 1,_RA.length()).toFloat()*10;
        }else{
          RAse = _RA.substring(k1 + 1,_RA.length()).toFloat();          
        }
        OBJECT_RA_M = _RA.substring(k0 + 1,k1).toFloat() + RAse / 60;
        Serial3.print("1");
      }
     // :Sd +18Я12:30#  - Set Target DEC
      if (BT_COMMAND_STR.indexOf("Sd")>0){
        // LX200 - DEC Coordinates ()

        int i = BT_COMMAND_STR.indexOf("Sd");
        String _DEC = BT_COMMAND_STR.substring(i+2,BT_COMMAND_STR.length()-1);
        int k0=0;
        if (_DEC.indexOf(223) > 0){
          k0 = _DEC.indexOf(223);
        }else{
          k0 = _DEC.indexOf("*");
        }
        int k1 = _DEC.indexOf(":");
        OBJECT_DETAILS += ", _DEC:" + _DEC;
        OBJECT_DEC_D = _DEC.substring(0,k0).toFloat();
        float DECse = 0;
        if (_DEC.substring(k1 + 1,_DEC.length()).length()<2){
          DECse = _DEC.substring(k1 + 1,_DEC.length()).toFloat()*10;
        }else{
          DECse = _DEC.substring(k1 + 1,_DEC.length()).toFloat();          
        }
        OBJECT_DEC_M = _DEC.substring(k0 + 1,k1).toFloat() + DECse / 60;
        if (OBJECT_DEC_D < 0){
          OBJECT_DEC_M *= -1;
        }
        Serial3.print("1");
      }
      // :MS#  -- Slew To Target RA and DEC
      if (BT_COMMAND_STR==":MS"){
           // LX200 - Slew To (previously sent coordinates with Sr & Sd)

           // Now SlewTo the selected object and draw information on mainScreen
           calculateLST_HA();
            if (ALT > 0){
                Serial3.print("0");   // 0 means that everything is OK
                if (IS_SOUND_ON){
                  SoundOn(note_C,32);
                  delay(200);
                  SoundOn(note_C,32);
                  delay(200);
                  SoundOn(note_C,32);
                  delay(1500);
                }
                UpdateObservedObjects();
                OBJECT_NAME = "BlueTooth";
                OBJECT_DESCR = "using LX200 Protocol";
                OBJECT_DETAILS = "Missing information about the object! " + OBJECT_DETAILS;

                // Stop Interrupt procedure for tracking.
                Timer3.stop(); // 
                IS_TRACKING = false;
                
                IS_OBJ_FOUND = false;
                IS_OBJECT_RA_FOUND = false;
                IS_OBJECT_DEC_FOUND = false;
                Slew_timer = millis();
                Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            }else{
                Serial3.print("1rDUINO Scope: Object Below Horizon! #");
            }
           drawMainScreen();
           BT_COMMAND_STR = "";
      }
      if (BT_COMMAND_STR=="Current"){
            Serial3.println("MECHANICS DATA (Software Defined):");
            Serial3.println("==================================");
            Serial3.print("Firmware Version: ");
            Serial3.println(FirmwareName + " " + FirmwareNumber);

            Serial3.print("WORM GEAR Tooths = ");
            Serial3.println(WORM);
            Serial3.print("REDUCTOR = 1:");
            Serial3.println(REDUCTOR);
            Serial3.print("MOTOR STEPS / REV = ");
            Serial3.println(DRIVE_STP);
            Serial3.print("MICROSteps Mode: ");
            Serial3.println(MICROSteps);
            
            Serial3.println("\r\nCALCULATED VALUES IN THE SOFTWARE:");
            Serial3.print("MicroSteps for 360 rotation = ");
            Serial3.println(MicroSteps_360);
            Serial3.print("RA_90 = ");
            Serial3.println(RA_90);
            Serial3.print("DEC_90 = ");
            Serial3.println(DEC_90);
            Serial3.print("RA_H_CONST = ");
            Serial3.println(HA_H_CONST);
            Serial3.print("DEC_D_CONST = ");
            Serial3.println(DEC_D_CONST);
            Serial3.print("RA Clock Motor (Micro Seconds) = ");
            Serial3.print(Clock_Sidereal);
            Serial3.print(" (");
            Serial3.print(1000000/Clock_Sidereal);
            Serial3.println(" MicroSteps / Second )");
            Serial3.print("Time to Meridian Flip = ");
            Serial3.print(mer_flp);
            Serial3.println(" hours");
            Serial3.println("\r\nCURRENT TELESCOPE POSITION COUNTER:");
            Serial3.print("RA_microSteps: ");
            Serial3.println(RA_microSteps);
            Serial3.print("DEC_microSteps: ");
            Serial3.println(DEC_microSteps);
            Serial3.print("delta_a_RA: ");
            Serial3.println(delta_a_RA);
            Serial3.print("delta_a_DEC: ");
            Serial3.println(delta_a_DEC);
            Serial3.println("\r\nCURRENT GPS POSITION & DATE:");
            Serial3.print("OBSERVATION LONGITUDE: ");
            Serial3.println(OBSERVATION_LONGITUDE, 6);
            Serial3.print("OBSERVATION LATTITUDE: ");
            Serial3.println(OBSERVATION_LATTITUDE, 6);
            Serial3.print("OBSERVATION ALTITUDE: ");
            Serial3.println(OBSERVATION_ALTITUDE, 6);
            Serial3.print("TIME ZONE: ");
            Serial3.println(TIME_ZONE);
            Serial3.print("Mount Date: ");
            Serial3.println(String(rtc.getDateStr()).substring(0,2)+" "+rtc.getMonthStr(FORMAT_SHORT)+" "+String(rtc.getDateStr()).substring(6));
            Serial3.print("Mount Time: ");
            Serial3.println(String(rtc.getTimeStr()).substring(0,5));
            Serial3.print("Summer Time: ");
            Serial3.println(Summer_Time);
            
            Serial3.println("\r\nOBJECT DATA (Selected Object):");
            Serial3.println("==================================");
            Serial3.print("IS_OBJ_FOUND = ");
            Serial3.println(IS_OBJ_FOUND);
            Serial3.print("OBJECT_NAME = ");
            Serial3.println(OBJECT_NAME);
            Serial3.print("RA = ");
            Serial3.print(OBJECT_RA_H,0);
            Serial3.print("h ");
            Serial3.print(OBJECT_RA_M);
            Serial3.println("m");
            Serial3.print("DEC = ");
            Serial3.print(OBJECT_DEC_D,0);
            Serial3.print("* ");
            Serial3.print(OBJECT_DEC_M);
            Serial3.println("m");
            Serial3.print("HA = ");
            Serial3.print(HAHour,0);
            Serial3.print("h ");
            Serial3.print(HAMin);
            Serial3.println("m");
            Serial3.print("ALT = ");
            Serial3.println(ALT,2);
            Serial3.print("AZ = ");
            Serial3.println(AZ,2);
            Serial3.println("\r\nVARIABLE DATA (Software):");
            Serial3.println("==================================");
            Serial3.print("IS_OBJ_VISIBLE: ");
            Serial3.println(IS_OBJ_VISIBLE);
            Serial3.print("IS_IN_OPERATION: ");
            Serial3.println(IS_IN_OPERATION);
            Serial3.print("IS_TRACKING: ");
            Serial3.println(IS_TRACKING);
            Serial3.print("IS_NIGHTMODE: ");
            Serial3.println(IS_NIGHTMODE);
            Serial3.print("IS_MERIDIAN_PASSED: ");
            Serial3.println(IS_MERIDIAN_PASSED);
            Serial3.print("IS_BT_MODE_ON: ");
            Serial3.println(IS_BT_MODE_ON);
            Serial3.print("CURRENT_SCREEN: "); 
            Serial3.println(CURRENT_SCREEN); 
            Serial3.println("==================================");
            Serial3.println("Bluetooth Commands:");
            Serial3.println(BTs);
                     
            BT_COMMAND_STR = "";            
        }
        if (BT_COMMAND_STR.indexOf("SlewTo")>0){
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
           if (BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length()) != "0" && BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length()) != ""){
              OBJECT_DETAILS += " and is a.k.a "+BT_COMMAND_STR.substring(i11+1, BT_COMMAND_STR.length())+".";
           }
           OBJECT_RA_H = BT_COMMAND_STR.substring(i2+1, i3).toFloat();
           OBJECT_RA_M = BT_COMMAND_STR.substring(i3+1, i4).toFloat();
           OBJECT_DEC_D = BT_COMMAND_STR.substring(i4+1, i5).toFloat();
           OBJECT_DEC_M = BT_COMMAND_STR.substring(i5+1, i6).toFloat();
           if (OBJECT_DEC_D  < 0){
             OBJECT_DEC_M *= -1;
           }

           
           // Now SlewTo the selected object and draw information on mainScreen
           calculateLST_HA();
            if (ALT > 0){
               if (IS_SOUND_ON){
                  SoundOn(note_C,32);
                  delay(200);
                  SoundOn(note_C,32);
                  delay(200);
                  SoundOn(note_C,32);
                  delay(1500);
               }
                UpdateObservedObjects();
                // Stop Interrupt procedure for tracking.
                Timer3.stop(); // 
           
                IS_TRACKING = false;                
                IS_OBJ_FOUND = false;
                IS_OBJECT_RA_FOUND = false;
                IS_OBJECT_DEC_FOUND = false;
                Slew_timer = millis();
                Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            }
           drawMainScreen();
           BT_COMMAND_STR = "";
        }
        if (BT_COMMAND_STR=="Status"){
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

          // Draw initial screen - INITIALIZE
          // The below part cannot be removed form the code
          // You can add messages, but not remove!
           Serial3.println("rDUINO SCOPE - TELESCOPE GOTO System");  
           Serial3.println("Copyright (C) 2016 Dessislav Gouzgounov");
           Serial3.println("Download for free @ http://rduinoscope.byethost24.com\r\n");
           Serial3.print("Observing Sesssion started on ");  
           Serial3.print(Start_date);
           Serial3.print(" @");
           Serial3.print(START_TIME);
           Serial3.println("h ");
           Serial3.print("Report generated at: ");
           Serial3.println(rtc.getTimeStr());
           Serial3.println("===============================================");
           Serial3.println("Location Information:"); 
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
              BT_COMMAND_STR = "";
            }
        }  
    // }
    BT_COMMAND_STR = "";
    Serial3.flush();
}

