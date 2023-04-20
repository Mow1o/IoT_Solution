#include "mbed.h"
// SPI LIBRARY
#include "SPI.h"

// NETWORKING
#include "ESP8266Interface.h"
#include "MQTTClientMbedOs.h"
//#include "MQTTNetwork.h"
// PMOD OLED LIBRARIES
#include "Adafruit_GFX.h" // we will get similar code working than in Arduino-boards.
#include "Adafruit_SSD1331.h" // By using the Adafruit SSD1331 library and Adafruit GFX library


// MAIN MBED LIBRARY

// LEDS
DigitalOut ledYellow(D9);
// DigitalOut ledGreen(D4);
// DigitalOut ledRed(D5);
// DigitalOut ledWhite(A1);
DigitalOut LED(D1);

// MICROPHONE
void getMicSound();
SPI mic(NC, D12, A4); // SET SPI
DigitalOut micCS(A3);

int raw = 0; // 16 bits from MIC3
int sound32bit = 0;
int sound = 0;

char volume[100];
int i = 1;
//char buffer[100];

// PMOD OLED
Adafruit_SSD1331 OLED(A7, A6, D10, D11, NC, D13); // cs, res, dc, mosi, (nc), sck
DigitalOut VCCEN(D2);
DigitalOut PMODEN(D3);

// Definition of colours on the OLED display
#define Black 0x0000
#define Blue 0x001F
#define Red 0xF800
#define Green 0x07E0
#define Cyan 0x07FF
#define Magenta 0xF81F
#define Yellow 0xFFE0
#define White 0xFFFF

void BlueEmptyScreen();

// BUFF SIZE
#define BUFF_SIZE 128

// TIME
char Time[32];
void getTime();

// CONNECTION

bool connection_setup = false;

  // Store device IP
SocketAddress deviceIP;                                                   
  // Store broker IP
SocketAddress MQTTBroker;

TCPSocket socket;

//ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);

void connectWiFi(ESP8266Interface *esp, bool *connected);
void pubMQTT(ESP8266Interface *esp);
// MQTT Protocol

// Microphone



// MAIN PROGRAM
int main() {
  LED = 0;
  VCCEN = 1;
  PMODEN = 1;
  // ThisThread::sleep_for(2000ms);
  LED = 1;
  // ThisThread::sleep_for(2000ms);
  LED = 0;

  set_time(1614069522);

  OLED.begin();
  BlueEmptyScreen();

  bool connected = false;
  ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);
  connectWiFi(&esp, &connected);
  // ThisThread::sleep_for(1000ms);
  if (connected) {
    Thread pubMQTTThread;
    pubMQTTThread.start(callback(pubMQTT, &esp));
    ThisThread::sleep_for(1000ms);
    while (1) {
      // do other stuff here if needed
    }
  } else {
    // handle connection error here
  }
}

void getTime() {
  time_t seconds = time(NULL); //  https://os.mbed.com/docs/mbed-os/v6.7/apis/time.html
  strftime(Time, 32, "%I:%M:%p\n", localtime(&seconds));
  // printf("Recorded :%s \r\n", Time); // in mbed OS 6.7 interferes with
  
}

void getMicSound() {
    // The function for reading the digital value from the mic
    micCS.write(1); // DESELECT

    mic.format(16, 0);
    mic.frequency(1000000);
    ThisThread::sleep_for(100ms);

    micCS.write(0); // SELECT
    ThisThread::sleep_for(1ms);

    raw = mic.write(0x0000); // Dummy data
    ThisThread::sleep_for(1ms);
    micCS.write(1); // DESELECT
    //printf("16 bits MIC3 = 0x%X", raw);

    sound32bit = raw << 22; // 22 bits to the left to create 32 bit two's complement
    sound = sound32bit / 16777216; // 2 exp24 = 16 7777 216  means shifting 24
                                 // bits left without shifting the sign!
                              
    sprintf(volume, "sound 12 bit = %d", sound);
    //ThisThread::sleep_for(1000ms);
}


void BlueEmptyScreen() {
  OLED.clearScreen();
  OLED.fillScreen(Blue);
  OLED.setTextColor(Cyan);
}

void connectWiFi(ESP8266Interface *esp, bool *connected) {
  SocketAddress deviceIP;

  printf("\nConnecting wifi..\n");
  int ret = esp->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
  if (ret != 0) {
    printf("\nConnection error\n");
    *connected = false;
  } else {
    printf("\nConnection success\n");
    *connected = true;
    esp->get_ip_address(&deviceIP);
    printf("IP via DHCP: %s\n", deviceIP.get_ip_address());
  }
  
}

void pubMQTT(ESP8266Interface *esp) {
    while (1) {
        // Store broker IP and connect to MQTT broker
        TCPSocket socket;
        SocketAddress MQTTBroker;

        MQTTClient client(&socket);
        esp->gethostbyname(MBED_CONF_APP_MQTT_BROKER_HOSTNAME, &MQTTBroker, NSAPI_IPv4, "esp");
        MQTTBroker.set_port(MBED_CONF_APP_MQTT_BROKER_PORT);
        socket.open(esp);
        socket.connect(MQTTBroker);
        
        // Connect to MQTT broker
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = 3;
        char *id = MBED_CONF_APP_MQTT_ID;
        data.clientID.cstring = id;
        client.connect(data);

        // Publish message to MQTT broker
        char buffer[64];
        getTime();
        getMicSound();
        snprintf(buffer, sizeof(buffer), "{\"time\":\"%s\", \"sound\":\"%d\"}", Time, sound);
        printf("%s", buffer);
        MQTT::Message message;
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buffer;
        message.payloadlen = strlen(buffer)+1;
        client.publish("Lokpt5562/245552/yes1", message);

        // Disconnect from MQTT broker and wait for next publish
        client.disconnect();
        ThisThread::sleep_for(1000ms);
    }
}
