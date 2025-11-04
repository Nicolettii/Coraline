#include <DHT.h>
#include <WiFi.h>
#include <Keypad.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

const int dhtpin = 4;
const auto dhttype = DHT11;
DHT dht(dhtpin, dhttype);

const char* ssid = "Home";
const char* pswd = "MN141201";
const char* apikey = "Projeto1MC";
const char* server = "http://tcc2025-473213.rj.r.appspot.com/api/leituras/";

const int keyrows = 4;
const int keycols = 4;
byte colpins[keycols] = { 27, 23, 14, 19 };
byte rowpins[keyrows] = { 32, 33, 25, 26 };
const char keys[keyrows][keycols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '#', '0', '*', 'D' }
};
Keypad kp = Keypad(makeKeymap(keys), rowpins, colpins, keyrows, keycols);

const int lcdcols = 16;
const int lcdrows = 2;
const int lcdaddr = 0x27;
LiquidCrystal_I2C lcd(lcdaddr, lcdcols, lcdrows);

struct sensors {
  String type;
  float (*read)();
};

float readtemp() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    temp = -999;
  }
  return temp;
}

float readhumi() {
  float humi = dht.readHumidity();
  if (isnan(humi)) {
    humi = -999;
  }
  return humi;
}

sensors sensorslist[] = {
  { "Temperatura", readtemp },
  { "Umidade", readhumi }
};

const int totalSensors = sizeof(sensorslist) / sizeof(sensorslist[0]);

char dmy[11];
char hms[9];

unsigned long lastpost = 0;
unsigned long interval = 60000;

enum state : unsigned char { boot, wifi, httpost, readth, error };
state currentstate = boot;

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop() {
  switch (currentstate) {
    case boot:
      bootsta();
      break;
    case wifi:
      wifista();
      break;
    case httpost:
      httpoststa();
      break;
    case readth:
		  readthsta();
		  break;
    case error:
      errorsta();
      break;
  }
}

void bootsta() {
  lcd.setCursor(0, 0);
  lcd.print("Press 1 to conne");
  lcd.setCursor(0, 1);
  lcd.print("ct into WiFi!");
  char input = kp.getKey();
  if (input == '1') {
    lcd.clear();
    currentstate = wifi;
  }
}

void wifista() {
  int attempt = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED && attempt < 5) {
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wait.");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("Wait..");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("Wait...");
    attempt++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wifi ok!");
    delay(1000);
    currentstate = httpost;
  } else {
    lcd.clear();
    lcd.print("Wifi failed.");
    delay(1000);
    currentstate = error;
  }
}

void httpoststa() {
  timeconfig();
  HTTPClient http;
  http.begin(server);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("X-API-KEY", apikey);

  String postData =
    "data=" + String(dmy) + "&time=" + String(hms) + "&totalSensors=" + String(totalSensors);

  for (int i = 0; i < totalSensors; i++) {
    float reading = sensorslist[i].read();
    postData += "&sensor" + String(i + 1) + "=" + sensorslist[i].type;
    postData += "&value" + String(i + 1) + "=" + String(reading, 2);
  }

  int responseCode = http.POST(postData);
  Serial.println("POST Data: " + postData);
  Serial.println("HTTP Response: " + String(responseCode));

  String responseBody = http.getString();
  Serial.println("Server Response: " + responseBody);

  if (responseCode == 200) {
    currentstate = readth;
  } else {
    delay(1000);
    currentstate = error;
  }
}

void timeconfig() {
  configTime(-6 * 3600, 0, "pool.ntp.org");
  struct tm timeinfo;
  int attempt = 0;
  while (!getLocalTime(&timeinfo) && attempt < 10) {
    delay(1000);
    attempt++;
  }
  if (attempt > 10) {
    strcpy(dmy, "00-00-0000");
    strcpy(hms, "00:00:00");
    return;
  } else {
    strftime(dmy, sizeof(dmy), "%d-%m-%Y", &timeinfo);
    strftime(hms, sizeof(hms), "%H:%M:%S", &timeinfo);
  }
}

void readthsta() {
	unsigned long now = millis();
	if (now - lastpost >= interval) {
		currentstate = httpost;
		lastpost = now;
		return;
	}
	
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(readtemp());
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("U:");
  lcd.print(readhumi());
  lcd.print("%");
  delay(3000);
}

void errorsta() {
  WiFi.disconnect();
  lcd.setCursor(0, 0);
  lcd.print("Something went w");
  lcd.setCursor(0, 1);
  lcd.print("rong.");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Returning to the");
  lcd.setCursor(0, 1);
  lcd.print("boot screen");
  delay(2000);
  lcd.clear();
  currentstate = boot;
}