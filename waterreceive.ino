#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <nRF24L01.h>
#include <RF24.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NRF
RF24 radio(4,5);
const byte address[6] = "00001";

// LEDs
#define RED_LED     33
#define YELLOW_LED  18
#define GREEN_LED   17
#define BUZZER      16

struct DataPacket
{
  float distance;
  float waterLevel;
  float percent;
};

DataPacket data;

void setup()
{
  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Wire.begin(21,22);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while(true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(5,20);
  display.println("DAM");
  display.println("SYSTEM");
  display.display();

  delay(2000);

  radio.begin();
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop()
{
  if(radio.available())
  {
    radio.read(&data,sizeof(data));

    Serial.println("-----------------------");
    Serial.print("Distance : ");
    Serial.println(data.distance);

    Serial.print("Water Level : ");
    Serial.println(data.waterLevel);

    Serial.print("Percentage : ");
    Serial.println(data.percent);

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Water:");
    display.print(data.percent,1);
    display.println("%");

    display.print("Level:");
    display.print(data.waterLevel,1);
    display.println(" cm");

    display.print("Gap:");
    display.print(data.distance,1);
    display.println(" cm");

    if(data.percent <= 60)
    {
      digitalWrite(GREEN_LED,HIGH);
      digitalWrite(YELLOW_LED,LOW);
      digitalWrite(RED_LED,LOW);
      noTone(BUZZER);

      display.setTextSize(2);
      display.setCursor(5,40);
      display.println("SAFE");
    }

    else if(data.percent <= 85)
    {
      digitalWrite(GREEN_LED,LOW);
      digitalWrite(YELLOW_LED,HIGH);
      digitalWrite(RED_LED,LOW);
      noTone(BUZZER);

      display.setTextSize(2);
      display.setCursor(0,40);
      display.println("ALERT");
    }

    else
    {
      digitalWrite(GREEN_LED,LOW);
      digitalWrite(YELLOW_LED,LOW);
      digitalWrite(RED_LED,HIGH);
      tone(BUZZER,2000);

      display.setTextSize(2);
      display.setCursor(0,40);
      display.println("FLOOD!");
    }

    display.display();
  }
}