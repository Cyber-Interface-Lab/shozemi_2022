#include <M5Stack.h> //ver 0.2.9
#include <WiFi.h>
#include <WiFiUdp.h>
#include "bala.h"

const char *ssid = "m5stack_server";   // 9文字以上。SoftAPモードM5StackのSSIDに書き換えてください
const char *password = "serverserver"; // 9文字以上。SoftAPモードM5Stackのパスワードに書き換えてください

const char *to_udp_address = "192.168.4.2"; //送信先のIPアドレス
const int to_udp_port = 55556;              //送信相手のポート番号
const int my_server_udp_port = 55555;       //開放する自ポート

WiFiUDP udp;

int right_speed = 0;
int left_speed = 0;

uint8_t received_value_red = 1;  //受信したボタンの状態
uint8_t received_value_blue = 1; //受信したボタンの状態

Bala bala;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  M5.begin(true, false, false, true);
  Serial.println("--------- M5 began ---------");
  setupWiFiUDPserver();
  delay(1000);
}

void loop()
{
  receiveUDP();
  updateSpeed();
  Serial.println("looping");
}

void receiveUDP()
{
  int packetSize = udp.parsePacket();
  if (packetSize > 0)
  {
    received_value_red = udp.read();
    received_value_blue = udp.read();
  }
}

void updateSpeed()
{
  if (received_value_red)
  {
    right_speed = 0;
  }
  else
  {
    right_speed = -300;
  }

  if (received_value_blue)
  {
    left_speed = 0;
  }
  else
  {
    left_speed = -300;
  }

  bala.SetSpeed(left_speed, right_speed);
  displayStatus(left_speed, right_speed);
}

void displayStatus(int left_speed, int right_speed)
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setCursor(70, 0, 4);
  M5.Lcd.printf("left speed: %d\n", left_speed);
  M5.Lcd.printf("right speed: %d\n", right_speed);
}

void setupWiFiUDPserver()
{
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true, true); // WiFi config設定リセット
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("WiFi connected!");
  Serial.print("My IP address: ");
  Serial.println(myIP);
  udp.begin(myIP, my_server_udp_port);
  delay(1000);
}
