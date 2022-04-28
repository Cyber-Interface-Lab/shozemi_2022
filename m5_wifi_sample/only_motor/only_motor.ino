#include <M5Stack.h> //ver 0.2.9
#include "bala.h"

Bala bala;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  M5.begin(true, false, false, true);
  Serial.println("--------- M5 began ---------");
  delay(1000);
}

void loop()
{
  Serial.println("setting speed");
  bala.SetSpeed(500, 500);
  Serial.println("set speed");
  delay(1000);
}