#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);

#include <DHT.h>
#define DHTTYPE DHT11
uint8_t DHTPin = D4;
DHT dht(DHTPin, DHTTYPE);
float Temperature, Humidity;

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <RTClib.h>
RTC_DS3231 rtc;

// define wifi credentials
const char *ssid     = "WIFI SSID";
const char *password = "WIFI PASSWD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ro.pool.ntp.org", 7200, 60000);

#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.swap(); //now hardware serial on D8/GPIO15(TX)-DF(RX) & D7/GPIO13(RX)-DF(TX)
  Serial.flush(); //clear serial library buffer, ALWAYS CLEAN BUFFER AFTER SERIAL SWAP!!!
  
  mp3.begin(Serial);
  mp3.volume(10);

  pinMode(DHTPin, INPUT);
  dht.begin();
  
  lcd.begin(16, 2);
  lcd.clear();

  if (! rtc.begin()) {
    lcd.print("Couldn't find RTC");
  }

if (rtc.lostPower()) {
    lcd.print("RTC lost power");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  syncTime();

}

void loop() {
  // put your main code here, to run repeatedly:

  Temperature = dht.readTemperature();
  delay(10);
  Humidity = dht.readHumidity();
  
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  
  DateTime now = rtc.now();
  unsigned int nw = (unsigned int)now.hour() * 10000 + (unsigned int)now.minute() * 100 + (unsigned int)now.second();


  if (now.day() < 10) lcd.print('0');
  lcd.print(now.day());
  lcd.print('/');
  if (now.dayOfTheWeek() == 0) lcd.print('7');
  else lcd.print(now.dayOfTheWeek());

  lcd.print(" ");
  //if (nw < 99999) lcd.print('0');
  //lcd.print(nw);

  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());
  

  lcd.setCursor(0, 1);  
  
  lcd.print(Temperature, 1);
  lcd.print(char(223)); // degree symbol
  lcd.print("C ");
  lcd.print(Humidity, 0);
  lcd.print("%RH");

  if ((uint32_t)millis() / 1000ul % 86400ul == 0ul){
    syncTime();
  }


  if (now.dayOfTheWeek() != 0){
    switch(nw){
      case 64000: mp3.playMp3Folder(1); break;
      case 65900: mp3.playMp3Folder(6); break;
      case 72000: mp3.playMp3Folder(7); break;
      case 73000: mp3.playMp3Folder(8); break;
      case 74000: mp3.playMp3Folder(9); break;
      case 74700: mp3.playMp3Folder(10); break;
      case 75200: mp3.playMp3Folder(5); break;
      default: break;
    }
  } // if (now.dayOfTheWeek() != 0){
  
  delay(500);
}

void syncTime(){
  showInitMessageLine("WiFi init .");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    lcd.print ( "." );
  }

  timeClient.begin();

  showInitMessageLine("Adjust time...");
  timeClient.update();
  rtc.adjust(timeClient.getEpochTime());
  WiFi.mode(WIFI_OFF);
  showInitMessageLine("Init done!");
  delay(2000);
  //lcd.clear();
  //lcd.setCursor(4,0);
}

void showInitMessageLine(String line){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line);
}
