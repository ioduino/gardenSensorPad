
#include <RF24Network.h>
#include "DHT.h"
#include <RF24.h>
#include <SPI.h>

RF24 radio(7,9);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t other_node = 00;       // Address of the other node in Octal format

const unsigned long interval = 2000; //ms  // How often to send 'hello world to the other unit



struct payload_t {                  // Structure of our payload
  float temp;
  float humidity;
  float moisture;
};

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define voltageFlipPin1 4
#define voltageFlipPin2 5
#define sensorPin 2

int flipTimer = 500;

void setSensorPolarity(boolean flip){
  if(flip){
    digitalWrite(voltageFlipPin1, HIGH);
    digitalWrite(voltageFlipPin2, LOW);
  }else{
    digitalWrite(voltageFlipPin1, LOW);
    digitalWrite(voltageFlipPin2, HIGH);
  }
}
void setup(void)
{
  Serial.begin(57600);
  //Serial.println("RF24Network/examples/helloworld_tx/");
 SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
  //SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  pinMode(voltageFlipPin1, OUTPUT);
  pinMode(voltageFlipPin2, OUTPUT);
  pinMode(sensorPin, INPUT);
  dht.begin();
}

void loop() {
  Serial.println("waiting on network update....");
  network.update();                          // Check the network regularly

  float h = dht.readHumidity();

  float t = dht.readTemperature();
  Serial.println(t);
  float hic = dht.computeHeatIndex(t, h, false);

  setSensorPolarity(true);
  delay(flipTimer);
  float val1 = analogRead(sensorPin);
  delay(flipTimer);  
  setSensorPolarity(false);
  delay(flipTimer);
  // invert the reading
   float val2 = 1023-analogRead(sensorPin);

   float avg = (val1 + val2) / 2;
   Serial.println(avg);
    //Serial.print("Sending...");
    payload_t payload = { t, h, avg };
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header,&payload,sizeof(payload));
    Serial.println("sent....waiting");
    delay(interval);
}


