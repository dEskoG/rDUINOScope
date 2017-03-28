
#rDUINOScope v2.1 Boiana EQ 
===========================================================
Release date: 28 March 2017
Status: official BETA
Author: Dessislav Gouzgounov (deskog@gmail.com)


This is the final release of the rDUINOScope software for Equatorial Mounts. There is a “jump” in versioning after latest V.1.8, because small improvements have been made for the past year, but those were never released to the general public, thus V.2.1 is a cumulative release.

PLEASE NOTE: Within v2.1 I promised support for AltAz / Dobsonian mounts. Later in the development I decided to split the code in 2 streams:
- #rDUINOScope v2.1 Boiana EQ  - Version for German Equatorial Mounts; 
- #rDUINOScope v2.1 Boiana AltAz – Version for AltAz / Dobsonian Mounts;

By the time of this release the AltAz version is ready but not tested, thus it will be released a bit later!


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
