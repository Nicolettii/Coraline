#include <DHT.h>
#include <Time.h>
#include <WiFi.h>
#include <Keypad.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define ssid "Home"
#define pswd "MN141201"
#define apikey "Projeto1MC"
#define server "http://tcc2025-473213.rj.r.appspot.com/api/leituras/"

#define dhtpin 4
#define dhtype DHT11
DHT dht(dhtpin, dhtype);

#define keyrows 4
#define keycols 4
byte colspins[keycols] = { 27, 23, 14, 19 };
byte rowspins[keyrows] = { 32, 33, 25, 26 };
const char keys[keyrows][keycols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '#', '0', '*', 'D' }
};

Keypad kp = Keypad(makeKeymap(keys), rowspins, colspins, keyrows, keycols);

#define lcdcol 16
#define lcdrow 2
#define addr 0x27
LiquidCrystal_I2C lcd(addr, lcdcol, lcdrow);

struct sensors {
  String type;
  float (*read)();
};

float readtemp() {
  delay(2000);
  return dht.readTemperature();
}

float readhumi() {
  delay(2000);
  return dht.readHumidity();
}

sensors sensorslist[] = {
  { "Temperatura", readtemp },
  { "Umidade", readhumi }
};

const int totalSensors = sizeof(sensorslist) / sizeof(sensorslist[0]);

char ymd[11];
  char hms[9];

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  indexmsg();
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

void indexmsg() {
  lcd.setCursor(0, 0);
  lcd.print("Press 1 to conn-");
  lcd.setCursor(0, 1);
  lcd.print("ect into WiFi!");
  keyinput();
}

void wificonnect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);
  wifimsg();
}

void serversend() {
  getime();
  HTTPClient http;
  http.begin(server);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("X-API-KEY", apikey);

  String postData =
    "data=" + String(ymd) + "&time=" + String(hms) + "&totalSensors=" + String(totalSensors);

  for (int i = 0; i < totalSensors; i++) {
    float reading = sensorslist[i].read();
    postData += "&sensor" + String(i + 1) + "=" + sensorslist[i].type;
    postData += "&value" + String(i + 1) + "=" + String(reading, 2);
  }

  int responseCode = http.POST(postData);
  Serial.println("POST Data: " + postData);
  Serial.println("HTTP Response: " + String(responseCode));

  if (responseCode > 0) {
    String responseBody = http.getString();
    Serial.println("Server Response: " + responseBody);
  }
}

void wifimsg() {
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 5) {
    delay(1000);
    lcd.print("wait.");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("wait..");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("wait...");
    delay(1000);
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wifi ok!");
    delay(1000);
    serversend();
  } else {
    lcd.print("Wifi failed.");
  }
}

void getime() {
  
  configTime(-3 * 3600, 0, "pool.ntp.org");
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  strftime(ymd, sizeof(ymd), "%Y-%m-%d", &timeinfo);
  strftime(hms, sizeof(hms), "%H:%M:%S", &timeinfo);
}

void readth() {
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
}