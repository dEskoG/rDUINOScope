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
//  - Bluetooth communication (both ways);
//
void considerBTCommands(){
      SoundOn(note_c,8);

      if (BT_COMMAND_STR.indexOf("RD_priv")>0){
        Current_RA_DEC();
        Serial3.print("Hour Angle: ");
        Serial3.println(curr_RA_lz);
        Serial3.print("Declination: ");
        Serial3.print(curr_DEC_lz);
      }
      if (BT_COMMAND_STR.indexOf("GR")>0){
        Current_RA_DEC();
        Serial3.print(curr_RA_lz);
        Serial3.print("#");
      }
      if (BT_COMMAND_STR.indexOf("GD")>0){
        Current_RA_DEC();
        Serial3.print(curr_DEC_lz);
        Serial3.print("#");
      }
     // #:Q#:Sr 07:08:52#
      if (BT_COMMAND_STR.indexOf(":Sr")>0){
        // LX200 - RA Coordinates ()
        int i = BT_COMMAND_STR.indexOf("Sr");
        String _RA = BT_COMMAND_STR.substring(i+3,BT_COMMAND_STR.length()-1);
        int k0 = _RA.indexOf(":");
        int k1 = _RA.indexOf(":", k0+1);
        OBJECT_DETAILS = "_RA:" + _RA;
        OBJECT_RA_H = _RA.substring(0,k0).toFloat();
        OBJECT_RA_M = _RA.substring(k0 + 1,k1).toFloat() + (_RA.substring(k1 + 1,_RA.length()).toFloat() / 60);
        Serial3.print("1");
      }
     // :Sd +18Я12:30#
      if (BT_COMMAND_STR.indexOf(":Sd")>0){
        // LX200 - DEC Coordinates ()
        int i = BT_COMMAND_STR.indexOf("Sd");
        String _DEC = BT_COMMAND_STR.substring(i+3,BT_COMMAND_STR.length()-1);
        int k0 = _DEC.indexOf(223);
        int k1 = _DEC.indexOf(":");
        OBJECT_DETAILS += ", _DEC:" + _DEC;
        OBJECT_DEC_D = _DEC.substring(0,k0).toFloat();
        OBJECT_DEC_M = _DEC.substring(k0 + 1,k1).toFloat() + (_DEC.substring(k1 + 1,_DEC.length()).toFloat() / 60);
        Serial3.print("1");
      }
      // :MS#
      if (BT_COMMAND_STR.indexOf(":MS#")>0){
           // LX200 - Slew To (previously sent coordinates with Sr & Sd)

           // Now SlewTo the selected object and draw information on mainScreen
           calculateLST_HA();
            if (ALT > 0){
                Serial3.print("0");   // 0 means that everything is OK
                SoundOn(note_C,32);
                delay(200);
                SoundOn(note_C,32);
                delay(200);
                SoundOn(note_C,32);
                delay(1500);
                UpdateObservedObjects();
                OBJECT_NAME = "Stellarium";
                OBJECT_DESCR = "via Bluetooth";
                OBJECT_DETAILS = "Missing information for this object! " + OBJECT_DETAILS;

                IS_OBJ_FOUND = false;
                IS_OBJECT_RA_FOUND = false;
                IS_OBJECT_DEC_FOUND = false;
                Slew_timer = millis();
                Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            }else{
                Serial3.print("1rDUINO Scope: Object Below Horizon! #");
            }
           drawMainScreen();
      }
      if (BT_COMMAND_STR.indexOf("Current")>0){
            Serial3.println("MECHANICS DATA (Software Defined):");
            Serial3.println("==================================");
            Serial3.print("MicroSteps for 360 rotation = ");
            Serial3.println(MicroSteps_360);
            Serial3.print("RA_90 = DEC_90 = ");
            Serial3.println(RA_90);
            Serial3.print("RA_D_CONST = DEC_D_CONST = ");
            Serial3.println(DEC_D_CONST);
            Serial3.print("Clock_Motor (microSec) = ");
            Serial3.println(Clock_Motor);
            Serial3.print("Meridian Flip (time) = ");
            Serial3.println(mer_flp);
            Serial3.println("\r\nOBJECT DATA (Selected):");
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
            Serial3.print("HA = ");
            Serial3.print(HAHour,0);
            Serial3.print("h ");
            Serial3.print(HAMin);
            Serial3.println("m");
            Serial3.print("DEC = ");
            Serial3.print(OBJECT_DEC_D,0);
            Serial3.print("* ");
            Serial3.print(OBJECT_DEC_M);
            Serial3.println("m");
            Serial3.print("ALT = ");
            Serial3.println(ALT,2);
            Serial3.print("AZ = ");
            Serial3.println(AZ,2);
            Serial3.print("RA_microSteps: ");
            Serial3.println(RA_microSteps);
            Serial3.print("DEC_microSteps: ");
            Serial3.println(DEC_microSteps);
            Serial3.print("delta_a_RA: ");
            Serial3.println(delta_a_RA);
            Serial3.print("delta_a_DEC: ");
            Serial3.println(delta_a_DEC);
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
        }
        if (BT_COMMAND_STR.indexOf("Status")>0){
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
           Serial3.println("Copyright (C) 2016 Dessislav Gouzgounov");
           Serial3.println("Download for free @ http://rduinoscope.co.nf\r\n");
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
            }
        }  
    // }
    BT_COMMAND_STR = "";
    Serial3.flush();
}

