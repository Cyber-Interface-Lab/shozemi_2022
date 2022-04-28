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
TaskHandle_t task_handl; //マルチタスクハンドル定義

// Bala bala;

uint8_t receive_position_data = 0; //受信図形の座標位置
uint8_t receive_direction = 0;     //受信図形の動作方向変数
uint8_t receive_color_data = 0;    //受信図形のカラー番号
uint8_t old_line_data = 0;
uint8_t old_color_data = 0;
boolean isDisp_ok = false; //ディスプレイ表示フラグ
boolean isSet_send_data = false;
boolean isSend_rect_move = false; //図形動作開始フラグ
int16_t send_position = 0;        //送信図形の座標位置
uint8_t send_direction = 0;       //送信図形の動作方向変数
uint8_t send_color_num = 0;       //送信図形のカラー番号
uint32_t now_time = 0;
int16_t interval = 100;    // UDPデータ送信間隔
uint8_t rect_width = 63;   //図形の幅
uint8_t rect_height = 100; //図形の高さ
uint32_t color_data[7] = {TFT_WHITE,
                          TFT_RED,
                          TFT_GREEN,
                          TFT_BLUE,
                          TFT_YELLOW,
                          TFT_MAGENTA,
                          TFT_CYAN};

//********* core 1 task ************
void setup()
{
  Serial.begin(115200);
  delay(1000);
  setupWiFiUDPserver();
  xTaskCreatePinnedToCore(&taskDisplay, "taskDisplay", 8192, NULL, 10, &task_handl, 0);
  delay(500); //これ重要。別タスクでM5.begin関数が起動するまで待つ。
}

void loop()
{
  receiveUDP();
  if (isSend_rect_move)
    autoIncDec(interval);
  // bala.SetSpeed(500, 500);
  // Serial.println("bala set speed");
  sendUDP();
}
//******** core 0 task *************
void taskDisplay(void *pvParameters)
{
  M5.begin(true, false, false, true);
  Serial.println("--------- M5 began ---------");
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.fillRect(0, 50, rect_width, rect_height, TFT_WHITE);
  while (true)
  {
    M5.update(); // Update M5Stack button state
    // bala.SetSpeed(500, 500);
    if (isDisp_ok)
    {
      if (old_line_data != receive_position_data || old_color_data != receive_color_data)
      {
        if (receive_direction)
        {
          M5.Lcd.fillRect(old_line_data, 50, receive_position_data - old_line_data, rect_height, TFT_BLACK);
        }
        else
        {
          M5.Lcd.fillRect(receive_position_data + rect_width, 50, old_line_data + rect_width, rect_height, TFT_BLACK);
        }
        M5.Lcd.fillRect(receive_position_data, 50, rect_width, rect_height, color_data[receive_color_data]);
        old_line_data = receive_position_data;
        old_color_data = receive_color_data;
      }
      isDisp_ok = false;
    }
    button_action();
    delay(1);
  }
}
//************************************
void receiveUDP()
{
  if (!isDisp_ok)
  {
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      receive_position_data = udp.read();
      receive_direction = udp.read();
      receive_color_data = udp.read();
      // Serial.println(receive_position_data);
      // Serial.printf("receive_position_data=%d, receive_color_data=%d\r\n", receive_position_data, receive_color_data);
      isDisp_ok = true;
    }
  }
}

void sendUDP()
{
  if (isSet_send_data)
  {
    udp.beginPacket(to_udp_address, to_udp_port);
    udp.write((uint8_t)send_position);
    udp.write(send_direction);
    udp.write(send_color_num);
    // Serial.printf("send_position=%d, send_color_num=%d\r\n", send_position, send_color_num);
    udp.endPacket();
    isSet_send_data = false;
  }
}

void autoIncDec(int16_t interval)
{
  if (millis() - now_time > interval)
  {
    if (send_direction)
    {
      send_position++;
      if (send_position > 255)
      {
        send_position = 255;
        send_direction = 0;
      }
    }
    else
    {
      send_position--;
      if (send_position < 0)
      {
        send_position = 0;
        send_direction = 1;
      }
    }
    isSet_send_data = true;
    now_time = millis();
  }
  // bala.SetSpeed(1000, 1000);
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

void button_action()
{
  if (M5.BtnA.wasReleased())
  {
    if (send_direction)
    {
      send_direction = 0;
    }
    else
    {
      send_direction = 1;
    }
    isSend_rect_move = true;
  }
  else if (M5.BtnB.wasReleased())
  {
    send_color_num++;
    if (send_color_num > 6)
      send_color_num = 0;
    isSet_send_data = true;
  }
  else if (M5.BtnC.wasReleased())
  {
    interval = interval - 5;
    if (interval < 0)
      interval = 0;
    Serial.printf("interval=%d\r\n", interval);
  }
  else if (M5.BtnC.pressedFor(500))
  {
    interval = 100;
    Serial.printf("interval=%d\r\n", interval);
    now_time = millis();
  }
}
