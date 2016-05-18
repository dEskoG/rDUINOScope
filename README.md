# rDUINOScope
rDUINOScope - Arduino based telescope control system (GOTO)

Published under GNU General Public License

Instructions on how to build hardware can be found on the project's website: http://rduinoscope.co.nf/

Facebook: https://www.facebook.com/rDUINOScope/


rDUINO SCOPE is Arduino Due based Telescope control system (a.k.a. GoTo). Basically it have a database with stellar objects, calculates their position on the sky and point's the telescope. Once the object is in the eyepiece, the system keeps tracking the object. 

The system works with stepper motors of all types and even works with the some commercially available products like SkyWatcher, MEADE, Orion, Vixen and etc.

rDUINO Scope utilizes the code and few sensors in order to collect needed data to control the telescope mount. As an output the rDUINO Scope controls 2 stepper motors representing RA and DEC movement of a German Equatorial Mount (GEM). The system only works with GEM and cannot control Alt/Az or Dobsonian mounts unless you add some code.


SENSORS:
- RTC DS3231 – Real-Time clock to hold the current date and time even when the system is not powered. The RTC DS3231 is a very accurate clock ±2 ppm - ±3.5 ppm, depending on the temperature, which creates variation of around ±2 min/year. I’m using the sensor in order to keep the time and later on calculate the time zone when calculating LST. (data: PDF);

- GPS uBlox Neo 6M - Is a GPS sensor, with pretty decent parameters of accuracy and time to resolve. So far I have only tested it in my house, thus the Cold start was a bit sluggish – 2-3 min. The hot start however is pretty fast – 10 sec. According to the datasheet it should be something around 30-40 sec for cold start, but this is about to be tested. (data: PDF);

- PS2 joystick – is a 5V joystick for Arduino. As the Arduino Due only supports 3.3V operation on each pin, the initial schematics I created (supplying 3.3V) was not performing well. I have developed a solution which can be seen on the schematics page. The main advantage of using Joystick rather than buttons to manual move RA and DEC is that based on the position of the joystick I assign different speeds, thus if you touch the joystick it moves slower than if you push it all the way down (full speed);

- Temperature & Humidity sensor DHT22 – Is also a pretty accurate temperature and humidity sensor with variations of about ±1 degree and humidity ±5%. I’m using this in order to provide the user with immediate information about the environment, but also log the data into the “statistics” screen and statistics sent via Bluetooth;

- Bluetooth module HC-05 – Used to assure communication with Cell Phone/ Tablet. 

- Stepper motor drivers DRV8825 – A stepper drivers allowing up to 2A per coil and utilizing 1/2, 1/4, 1/8, 1/16, 1/32 micro Steps. The mode of operation is selected based on the setup you have (mount parameters) and on the XLS file with calculations. Please, use the file to calculate your values and edit the code accordingly.




