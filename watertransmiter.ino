#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ---------------- NRF ----------------
RF24 radio(4, 5);      // CE, CSN
const byte address[6] = "00001";

// ------------- Ultrasonic -------------
#define TRIG_PIN 12
#define ECHO_PIN 14

// Tank height (change according to your tank/dam model)
float tankHeight = 24.0;

// Data structure
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

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

float readDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return -1;

  return duration * 0.0343 / 2;
}

void loop()
{
  float distance = readDistance();

  if (distance > 2 && distance < 400)
  {
    float waterLevel = tankHeight - distance;

    if (waterLevel < 0)
      waterLevel = 0;

    float percent = (waterLevel / tankHeight) * 100;

    if (percent > 100)
      percent = 100;

    data.distance = distance;
    data.waterLevel = waterLevel;
    data.percent = percent;

    radio.write(&data, sizeof(data));

    Serial.println("-------------------------");
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");

    Serial.print("Water Level : ");
    Serial.print(waterLevel);
    Serial.println(" cm");

    Serial.print("Tank Filled : ");
    Serial.print(percent);
    Serial.println("%");
  }

  delay(1000);
}