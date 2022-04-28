/*
    Description: Read the button status of DUAL BUTTON Unit and display it on the screen.
*/
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid = "m5stack_server"; //9文字以上。SoftAPモードM5StackのSSIDに書き換えてください
const char* password = "serverserver"; //9文字以上。SoftAPモードM5Stackのパスワードに書き換えてください
 
const char * to_udp_address = "192.168.4.1"; //送信先のIPアドレス
const int to_udp_port = 55555; //送信相手のポート番号
const int my_server_udp_port = 55556; //開放する自ポート
 
WiFiUDP udp;
boolean connected = false;
boolean isSet_send_data = false;

int last_value_red = 0;
int cur_value_red = 0;
int last_value_blue = 0;
int cur_value_blue = 0;

void setup() {
  // init lcd
  M5.begin();
  Serial.begin(115200);
  delay(1000);
  connectToWiFi();
  while(!connected){
    delay(1);
  }
  M5.Power.begin();
  pinMode(26, INPUT);
  pinMode(36, INPUT);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(PINK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("Wifi button controler");
  M5.Lcd.setTextColor(WHITE);
}

void loop() {
  isSet_send_data = true;
  sendUDP();
  cur_value_red = digitalRead(26);
  cur_value_blue = digitalRead(36);

  M5.Lcd.setCursor(0,25); M5.Lcd.print("Blue Status: ");
  M5.Lcd.setCursor(0,45); M5.Lcd.print("Blue Value: ");
  M5.Lcd.setCursor(0,65); M5.Lcd.print(" Red Status: ");
  M5.Lcd.setCursor(0,85); M5.Lcd.print(" Red Value: ");
  isSet_send_data = true;
  if(cur_value_blue != last_value_blue){
    // isSet_send_data = true;
    // sendUDP();
    M5.Lcd.fillRect(160,25,100,25,BLACK);
    M5.Lcd.fillRect(160,45,100,25,BLACK);
    if(cur_value_blue==0){
     M5.Lcd.setCursor(160,25); M5.Lcd.print("pressed");
     M5.Lcd.setCursor(160,45); M5.Lcd.print("0");
     Serial.println("Button Status: blue pressed");
     Serial.println(" value: 0");
    }
    else{
      M5.Lcd.setCursor(160,25); M5.Lcd.print("released");
      M5.Lcd.setCursor(160,45); M5.Lcd.print("1");
      Serial.println("Button Status: blue released");
      Serial.println(" value: 1");
    }
      last_value_blue = cur_value_blue;
  }

    if(cur_value_red != last_value_red){
      // isSet_send_data = true;
      // sendUDP();
      M5.Lcd.fillRect(160,65,100,25,BLACK);
      M5.Lcd.fillRect(160,85,100,25,BLACK);
      if(cur_value_red==0){
        M5.Lcd.setCursor(160,65); M5.Lcd.print("pressed");
        M5.Lcd.setCursor(160,85); M5.Lcd.print("0");
        Serial.println("Button Status: red pressed");
        Serial.println(" value: 0");
      }
      else{
        M5.Lcd.setCursor(160,65); M5.Lcd.print("released");
        M5.Lcd.setCursor(160,85); M5.Lcd.print("1");
        Serial.println("Button Status: red released");
        Serial.println(" value: 1");
      }
      last_value_red = cur_value_red;
  }

  M5.update();
}

 
void sendUDP(){
  if(isSet_send_data){
    udp.beginPacket(to_udp_address, to_udp_port);
    udp.write((uint8_t)cur_value_red);
    udp.write((uint8_t)cur_value_blue);
    //Serial.printf("send_position=%d, send_color_num=%d\r\n", send_position, send_color_num);
    udp.endPacket();
    isSet_send_data = false;
  }
}

void connectToWiFi(){
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, password);
  Serial.println("Waiting for WIFI connection...");
}
 
void WiFiEvent(WiFiEvent_t event){
  IPAddress myIP = WiFi.localIP();
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected!");
      Serial.print("My IP address: ");
      Serial.println(myIP);
      //udp.begin関数は自サーバーの待ち受けポート開放する関数である
      udp.begin(myIP, my_server_udp_port);
      delay(1000);
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
    default:
      break;
  }
}

 
