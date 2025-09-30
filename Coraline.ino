#include <DHT.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#define dhtpin 4
#define dhtype DHT11
DHT dht(dhtpin, dhtype);

#define keyrows 4
#define keycols 4
byte colspins[keycols] = {27, 23, 14, 19};
byte rowspins[keyrows] = {32, 33, 25, 26};
const char keys[keyrows][keycols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'#','0','*','D'}
};

Keypad kp = Keypad(makeKeymap(keys), rowspins, colspins, keyrows, keycols);

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
wifimsg();
}

void loop() {
keyinput();
}

void keyinput() {
  char input = kp.getKey();
  if (input == '1') {
    wificonnect();
  }
}

void wifimsg() {
  lcd.setCursor(0,0); 
  lcd.print("Press 1 to conn-");
  lcd.setCursor(0,1);
  lcd.print("ect into WiFi!");
}

void wificonnect() {
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("heyo");
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
