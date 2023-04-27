This program is coded with Mbed Studio


Libraries used:
 
mbed-mqtt master
Adafruit-GFX-MbedOS6
Adafruit_SSD1331_MbedOS6

Technical components:

L432KC Microcontroller
PMOD OLED RGB
PMOD MIC3
WiFi Mod ESP8226
2x LEDs

Technical specifications:

Starting the program it will make variable esp for the ESP8266Interface which will be used for making WiFi connection, getting the local stored IP address.

Inside the main function we set comments for each variable and being function called for easier reading of the program.


Error checking:

For each connection setup, there is a error check that will print the error code and it will print the proper result according to nsapi_error_t result.
"nsapi_error_t result" The codes and results can be found inside ESP8266Interface.h file if you want to study them more.
