This program is coded with Mbed Studio, it takes the value from PMOD MIC3 and sends it to MQTT broker.

Libraries used:
 
mbed-mqtt master

Technical components:

L432KC Microcontroller
PMOD MIC3
WiFi-Mod-ESP8226


Technical specifications:

Create a new program in Mbed and set target Microcontroller as L432KC, install libraries, set in mbed_app.json the values for WiFi & MQTT you are good to go.

Don't use WiFi-Mod-ESP8266 MOSI pin, leave it empty so it doesn't break the program.

Green led = connecting or sending message.
yellow led = program is on.
red led = error happened.

Error checking:

For each connection setup, there is a error check that will print the error code and it will print the proper result according to nsapi_error_t result.
"nsapi_error_t result" The codes and results can be found inside ESP8266Interface.h file if you want to study them more.
