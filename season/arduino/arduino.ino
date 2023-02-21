// M5StickCPlus library
// https://github.com/m5stack/M5StickC-Plus
#include <M5StickCPlus.h>

float pitch = 0.0F;
float roll = 0.0F;
float yaw = 0.0F;
float sensor_x = 0.0F;
float sensor_y = 0.0F;
float sensor_z = 0.0F;

float month = 0;

//  OSC library
//  https://github.com/CNMAT/OSC

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

// OSC
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
// #include "Ultrasonic.h"
 
// Ultrasonic ultrasonic(33);

WiFiUDP Udp;
const unsigned int _local_port = 8888;  // local port
const unsigned int _out_port = 9999;    //destination

OSCErrorCode error;

// WiFi
int address = 0;
String ssid = "ccbt_gakki_wifi";
String password = "deaihatotuzen";
String local_ip_str = "10.0.0.100";
String gateway_ip_str = "10.0.0.1";
String out_ip_str = "10.0.0.10";
String subnet_ip_str = "255.255.255.0";

enum Mode {
  SPRING, FALL
} mode = SPRING;
float vols[12];

IPAddress _local_ip;
IPAddress _gateway;
IPAddress _subnet;
IPAddress _out_ip;

// FreeRTOS
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void TaskOsc(void *pvParameters);
void TaskDisplay(void *pvParameters);

void setup() {
  M5.begin();             // Init M5StickC Plus
  M5.Imu.Init();          // Init IMU
  M5.Lcd.setRotation(3);  // Rotate the screen.
  M5.Lcd.fillScreen(BLACK);

  Serial.begin(115200);

  _local_ip.fromString(local_ip_str);
  _gateway.fromString(gateway_ip_str);
  _subnet.fromString(subnet_ip_str);
  _out_ip.fromString(out_ip_str);

  WiFi.mode(WIFI_STA);
  if (!WiFi.config(_local_ip, _gateway, _subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(_local_port);
  Serial.print("Local port: ");
  Serial.println(_local_port);

  xTaskCreateUniversal(
    TaskOsc, "TaskOsc", 8192, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreateUniversal(
    TaskDisplay, "TaskDisplay", 8192, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
}

// mapがうまく機能しなかったので、関数を用意しました
float mapRange (float value, float a, float b, float c, float d) {
    value = (value - a) / (b - a);
    return c + value * (d - c);
}

float getVolume (float value, float center, float range) {
    value = 1 - abs( value - center ) / range;
    if (value < 0) value = 0;
    return sin( value * HALF_PI);
}

uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue){
  return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);
}

// OSCタスク
void TaskOsc(void *pvParameters) {
  (void)pvParameters;
  while (1) {

    OSCMessage msg("/vol");

    // センサの範囲を、0〜1に整える
    
    M5.IMU.getAhrsData(&pitch, &roll, &yaw);
    roll = - roll + 15;    
    sensor_y = mapRange(roll, 32, 78, 0, 5.5);
    
    month = sensor_y;
    if (mode == SPRING)
    {
      //六月半分を過ぎたら
      if(month > 5.4)
      {
        mode = FALL;
      }
    } 
    else
    {
      month = 11 - month;
      if(month > 11)
      {
        mode = SPRING;
      }
    }
    if (month >= 0 && month < 12) {

      vols[0] = getVolume(month, 0.25, 0.5);
      vols[1] = getVolume(month, 1.25, 1.0);
      vols[2] = getVolume(month, 2, 1);
      vols[3] = getVolume(month, 3.5, 1);
      vols[4] = getVolume(month, 4, 0.7);
      vols[5] = getVolume(month, 4.7, 1.2);
      vols[6] = getVolume(month, 6.5, 1.5);
      vols[7] = getVolume(month, 7, 1);
      vols[8] = getVolume(month, 8.5, 1);
      vols[9] = getVolume(month, 9, 1.5);
      vols[10] = getVolume(month, 10, 1);
      vols[11] = getVolume(month, 11, 1);
      
    } else {
      vols[0] = vols[1] = vols[2] = vols[3] = vols[4] 
              = vols[5] = vols[6] = vols[7] = vols[8] 
              = vols[9] = vols[10] = vols[11] = 0;
    }
      msg.add(vols[0]).add(vols[1]).add(vols[2]).add(vols[3])
          .add(vols[4]).add(vols[5]).add(vols[6]).add(vols[7])
          .add(vols[8]).add(vols[9]).add(vols[10]).add(vols[11]);

      Udp.beginPacket(_out_ip, _out_port);
      msg.send(Udp);    // send the bytes to the SLIP stream
      Udp.endPacket();  // mark the end of the OSC Packet
      msg.empty();      // free space occupied by message
    vTaskDelay(100);
  }
}



// Displayタスク
void TaskDisplay(void *pvParameters) {
  (void)pvParameters;
  while (1) {  

    M5.Lcd.fillRect(10, 10, 30, 30,mode == SPRING ? getColor(255,20,147) : getColor(139,69,19));
    M5.Lcd.setCursor(10, 50);
    // M5.Lcd.printf(" %5.2f %5.2f %5.2f   ", pitch, roll, yaw);
    // M5.Lcd.setCursor(0, 100);
    M5.Lcd.setTextSize(3);
    M5.Lcd.printf("MONTH %5.2f", month+1);
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.setTextSize(1.5);
    M5.Lcd.printf("ROLL %5.2f", roll);

    vTaskDelay(10);
  }
}
void loop() {
}