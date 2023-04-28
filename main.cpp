// Mbed LIBRARY
#include "mbed.h"
#include <mutex>
#include <ctime>
// SPI LIBRARY
#include <SPI.h>
// NETWORKING
#include "ESP8266Interface.h"
#include <MQTTClientMbedOs.h>

#include <ctime>
// LED pins
DigitalOut LedYellow(A7); // Program is working correctly 
DigitalOut LedRed(D1); // Error has been occured.
DigitalOut LedGreen(D2); // WiFi setup, connection setup, publish

// MICROPHONE pins
SPI mic(D11, D12, D13); // SET SPI
DigitalOut micCS(D3);

//MICROPHONE function & global variables
 int raw = 0; // 16 bits from MIC3
 int sound32bit = 0;
 int sound = 0;
 char volume[64];
 char buffer[64];
bool connected = true;
void getMicSound();

// Definition of colours on the OLED display
#define Black 0x0000
#define Blue 0x001F
#define Red 0xF800
#define Green 0x07E0
#define Cyan 0x07FF
#define Magenta 0xF81F
#define Yellow 0xFFE0
#define White 0xFFFF

// TIME variable & function
char Time[32];
void getTime();

//Store device IP
SocketAddress deviceIP;
//Store broker IP
SocketAddress MQTTBroker;
// Store TCP socket
TCPSocket socket;

//Functions

void connectWiFi(ESP8266Interface *esp);
//void pubMQTT(ESP8266Interface *esp);
void netcheck(ESP8266Interface *esp);

// esp interface and set communication pins
ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);

// MAIN PROGRAM
int main() {

  LedYellow = 1;
  // MAIN VARIABLES //
  int i = 0;

  set_time(1614069522);
  // CONNECT SETUP function
  connectWiFi(&esp);


  //Create MQTTClient
  MQTTClient client(&socket);
  //Connection to test.mosquitto.org
  nsapi_error_t result = esp.gethostbyname(MBED_CONF_APP_MQTT_BROKER_HOSTNAME,&MQTTBroker, NSAPI_IPv4, "esp");
  if (result != NSAPI_ERROR_OK) {
    printf("Failed to get MQTT broker IP address (error code %d)\n", result);
    //Red light for showing something went wrong.
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
  } else {
    printf("MQTT broker IP archived\n");
    //Green light for showing everything OK.
    LedGreen = 1; 
    ThisThread::sleep_for(2000ms);
    LedGreen = 0;
  }

  // SET MQTT broker connection port from mbed_app.json
  MQTTBroker.set_port(MBED_CONF_APP_MQTT_BROKER_PORT);

  // Connect to MQTT broker
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  char *id = MBED_CONF_APP_MQTT_ID;
  data.clientID.cstring = id;

  MQTT::Message message;
  message.qos = MQTT::QOS0;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buffer;
  message.payloadlen = 64;

  result = socket.open(&esp);
  if (result != NSAPI_ERROR_OK) {
    printf("Failed to open socket (error code %d)\n", result);
    //Red light for showing something went wrong.
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
  } else {
    printf("Socket is open\n");
    //Green light for showing everything OK.
    LedGreen = 1; 
    ThisThread::sleep_for(2000ms);
    LedGreen = 0;
  }
  ThisThread::sleep_for(100ms);
  result = socket.connect(MQTTBroker);
  if (result != NSAPI_ERROR_OK) {
    printf("Failed to connect to MQTT broker (error code %d)\n", result);
    //Red light for showing something went wrong.
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
  } else {
    printf("Connected to the MQTT broker\n");
    //Green light for showing everything OK.
    LedGreen = 1; 
    ThisThread::sleep_for(2000ms);
    LedGreen = 0;
  }
  ThisThread::sleep_for(100ms);
  result = client.connect(data);
  if (result != 0) {
    printf("Failed to connect MQTT client (error code %d)\n", result);
    //Red light for showing something went wrong.
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
  } else {
    printf("Connected to the MQTT client\n");
    //Green light for showing everything OK.
    LedGreen = 1; 
    ThisThread::sleep_for(2000ms);
    LedGreen = 0;
  }
    
    while (i < 50 && connected) {
    i += 1;
    getMicSound(); // Get the value from microphone and print it to memory on "sound" variable.
    printf("%s\n", buffer);
    netcheck(&esp); // Network check to see connection is still up.
    result = client.publish(MBED_CONF_APP_MQTT_TOPIC, message);
    if (result != 0) {
    printf("Failed to publish the message (error code %d)\n", result);
    //Red light for showing something went wrong.
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
    ThisThread::sleep_for(2000ms);
    connected = false;
    } else {
    ThisThread::sleep_for(2000ms);
    printf("Published the message correctly\n");
    //Green light for showing everything OK.
    LedGreen = 1; 
    ThisThread::sleep_for(2000ms);
    LedGreen = 0;
    }
    sound = 0; // We set the sound variable back to 0 so while loop can continue inside getMicSound function.
    }
    LedYellow = 0;
    printf("MQTT connection disconnected\n");
    ThisThread::sleep_for(2000ms);
    client.disconnect();
    socket.close();
    LedRed = 1;

}
void getTime() {
  
  time_t seconds = time(NULL); //  https://os.mbed.com/docs/mbed-os/v6.7/apis/time.html
  strftime(Time, 32, "%I:%M:%p\n", localtime(&seconds));
  ThisThread::sleep_for(200ms);
  // printf("Recorded :%s \r\n", Time); // in mbed OS 6.7 interferes with
}
void getMicSound() {
    while(sound < 50) 
    {
    ThisThread::sleep_for(10ms);
    // The function for reading the digital value from the mic
    micCS.write(1); // DESELECT chip

    mic.format(16, 0);
    mic.frequency(1000000);
    ThisThread::sleep_for(100ms);

    micCS.write(0); // SELECT chip
    ThisThread::sleep_for(1ms);

    raw = mic.write(0x0000); // Dummy data
    ThisThread::sleep_for(1ms);
    micCS.write(1); // DESELECT
    // printf("16 bits MIC3 = 0x%X", raw)
    ThisThread::sleep_for(10ms);
    sound32bit = raw << 22; // 22 bits to the left to create 32 bit two's complement
    ThisThread::sleep_for(10ms);
    sound = sound32bit / 16777216; // 2 exp24 = 16 7777 216  means shifting 24
                                   // bits left without shifting the sign!
    printf("Sound value: %d\n ", sound);
    
    if (sound > 50) {
    getTime(); // Calling Time function to get the current time
    printf("Sound exceeds 20, send message to the broker\n ");
    sprintf(buffer, "\"Value from Microphone and Time: %d time: %s\"", sound, Time);
    }
    ThisThread::sleep_for(320ms);
    }
}

void netcheck(ESP8266Interface *esp) {
    nsapi_error_t result = esp->get_connection_status();
    if (result != NSAPI_ERROR_OK) {
    printf("Connection status: OK\n");
    }
    else {
    printf("No connection, connecting again: (error code %d)\n", result);
    connectWiFi(esp);
    }
}


void connectWiFi(ESP8266Interface *esp)
{
  printf("\nConnecting wifi..\n");

  int ret = esp->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD,NSAPI_SECURITY_WPA_WPA2);
  if (ret != 0) {
    printf("\nConnection error\n");
    LedRed = 1;
    ThisThread::sleep_for(1000ms);
    LedRed = 0;
    connected = false;
  } else {
    printf("\nConnection success\n");
    connected = true;
    LedGreen = 1;
    ThisThread::sleep_for(1000ms);
    LedGreen = 0;
    int getip = esp->get_ip_address(&deviceIP);
    printf("IP via DHCP: %s\n", deviceIP.get_ip_address());
  }

}
