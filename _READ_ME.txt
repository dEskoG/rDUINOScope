#rDUINOScope

THE WORLD'S FIRST STAND ALONE ARDUINO BASED TELESCOPE CONTROL GOTO (handheld controller). 

rDUINOScope is an Open Source, Arduino Due based Telescope Control System (GOTO). Drafted as stand alone system, rDUINOScope does not need PC, Tablet or Cell Phone, nor Internet connection in order to operate and deliver stunning views! It does however supports Bluetooth and LX200 protocol to connect and be controlled by smart devices!


rDUINOScope uses database with THE BEST ~250 stellar objects ( Messier and Hidden Treasures Catalogues) and 200 stars, calculates their position on the sky and points the telescope. Implemented Alignment procedures allow for "Always in the eyepiece" experience when slewing to selected objects and once the object is in the eyepiece, the system keeps tracking it.

The best part of rDUINOScope is that it is an OPEN SOURCE SOFTWARE and HARDWARE! Few custom telescope makers have announced that they are adding or about to add it to their product line. In addition, few months ago a new HW shield board was born as a spin-off project to spare you the hassle of building your own rDUINOScope!

Published under GNU General Public License

Instructions on how to build hardware can be found on the project's website: http://rduinoscope.byethost24.com/
HACKADAY.IO Project: https://hackaday.io/project/21024-rduinoscope

Facebook: https://www.facebook.com/rDUINOScope/


#rDUINOScope v2.3 Boiana EQ 
===========================================================
Release date: 10 June 2017
Status: RC
Author: Dessislav Gouzgounov / Äåñèñëàâ Ãóçãóíîâ /(deskog@gmail.com) 


This is the final version of the rDUINOScope software for Equatorial Mounts. There is a “jump” in versioning after  V.1.8 and latest v2.1, because small improvements have been made recently, but those were never released to the general public, thus V.2.3 is a cumulative release.

PLEASE NOTE: Within v2.1 I promised support for AltAz / Dobsonian mounts. Later in the development I decided to split the code in 2 streams:
- #rDUINOScope v2.3 Boiana EQ  - Version for German Equatorial Mounts; 
- #rDUINOScope v2.3 Boiana AltAz – Version for AltAz / Dobsonian Mounts;

By the time of this release the AltAz version is ready but not tested, thus it will be released a bit later!



New features added in v2.3:
===========================================================
- Code Optimization for Performance (SlewTo now capable of speeds > 3 deg/sec);
- Added Celestial, Lunar and Solar tracking speeds;
- Added new Main Menu system for more functionality;
- Added control on 2 external devices (DEV 1 & 2 - you can hook your Heater, Fan and etc);
- Added support for SkySafari 5 (smartphone software);
- Added build in Star Atlas showing current telescope position;
- Shows current Firmware version on Loading screen and on Bluetooth request using LX200 protocol;
- Bug fixes:
	o Wrong DEC movement for negative DEC values passed over Bluetooth;
	o LST revised in the format XX:XX;




New features added in v2.1:
===========================================================
- Added “1 Star Alignment” Method;
- Added “Iterative Alignment” Method;
- Added 200 brightest stars to be used in alignment procedures;
- Added Bluetooth control from Stellarium (cloned LX200 communication protocols);
- Added Bluetooth connection with PC;
- Code has been refactored for easy reading/editing;
- Bug fixes:
	o Wrong DEC movement for negative DEC values;
	o Revised RA movement formula;
	o OnScreen messages have been fixed;
	o 1px. Buttons move after press event was fixed;


