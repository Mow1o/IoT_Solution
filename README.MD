This program is coded with Mbed Studio

Libraries used:
 
mbed-mqtt master

Technical components:

L432KC Microcontroller
PMOD MIC3
WiFi-Mod-ESP8226


Technical specifications:

Don't use WiFi-Mod-ESP8266 MOSI pin, leave it empty so it doesn't break the program.

Error checking:

For each connection setup, there is a error check that will print the error code and it will print the proper result according to nsapi_error_t result.
"nsapi_error_t result" The codes and results can be found inside ESP8266Interface.h file if you want to study them more.
