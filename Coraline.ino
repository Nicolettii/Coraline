#include <DHT.h>

#define dhtpin 4
#define dhtype DHT11
DHT dht(dhtpin, dhtype);

void setup() {
Serial.begin(115200);
dht.begin();
}

void loop() {
read();
}

void read() {
delay(2000);
float temp = dht.readTemperature();
float humi = dht.readHumidity();
Serial.print("T:");
Serial.print(temp);
Serial.print((char)223);
Serial.println("C");

Serial.print("U:");
Serial.print(humi);
Serial.println("%");
}