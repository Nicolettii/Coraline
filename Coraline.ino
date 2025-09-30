#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define dhtpin 4
#define dhtype DHT11
DHT dht(dhtpin, dhtype);

#define lcdcol 16
#define lcdrow 2
#define addr 0x27
LiquidCrystal_I2C lcd(addr, lcdcol, lcdrow);

void setup() {
Serial.begin(115200);
dht.begin();
lcd.init();
lcd.clear();
lcd.backlight();
}

void loop() {
read();
}

void read() {
delay(2000);
float temp = dht.readTemperature();
float humi = dht.readHumidity();
lcd.setCursor(0, 0);
lcd.print("T:");
lcd.print(temp);
lcd.print((char)223);
lcd.print("C");

lcd.setCursor(0, 1);
lcd.print("U:");
lcd.print(humi);
lcd.print("%");
}