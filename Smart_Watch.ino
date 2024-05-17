#include "RTClib.h"// RTC library
#include <Adafruit_SSD1306.h> // oled display libaray
#include <Adafruit_GFX.h> // used for darw on the display
#include <SPI.h>
#include <Wire.h>
#include <dht.h> // DH11 sensor library

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // The (-1) parameter means that your OLED display doesn’t have a RESET pin.
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define dataPin 7 // sensor datapin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

dht DHT; // creat an object
RTC_DS1307 rtc; // creat an object

int selected = 0;
int entered = -1;

int down = digitalRead(2);
int up = digitalRead(3);

int reset = digitalRead(6);
int enter = digitalRead(4);
int back = digitalRead(5);

int setAlarmH = 0;
int setAlarmM = 0;
int setAlarmS = 0;

const char daysOfTheWeek[7][9] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void displayMenu() {

  down = digitalRead(2);
  up = digitalRead(3);

  enter = digitalRead(4);
  back = digitalRead(5);

  if (up == HIGH && down == HIGH) {};

  if (up == HIGH)
  {
    selected++;
    if (selected > 5) {
      selected = 0;
    }
    delay(150);
  }

  if (down == HIGH)
  {
    selected--;
    if (selected < 0) {
      selected = 5;
    }
    delay(150);
  }

  if (enter == HIGH)
  {
    entered = selected;
  }
  if (back == HIGH) {
    entered = -1;
  }

  const char * options[5] = {
    " Clock",
    " Alarm",
    " Temp and Humidity",
    " Edit Time",
    " Edit Date"
  };

  if (entered == -1)
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Alarm in: ");
    if (setAlarmH < 10) {
      display.print('0');
    }
    display.print(setAlarmH);
    display.print(":");
    if (setAlarmM < 10) {
      display.print('0');
    }
    display.print(setAlarmM);
    display.print(":");
    if (setAlarmS < 10) {
      display.print('0');
    }
    display.println(setAlarmS);
    display.println("");
    for (int i = 0; i < 5; i++) {
      if (i == selected) {
        display.setTextColor(BLACK, WHITE);
        display.println(options[i]);
      } else if (i != selected) {
        display.setTextColor(WHITE);
        display.println(options[i]);
      }
    }
  } else if (entered == 0) {
    date();
    clock();
  } else if (entered == 1) {
    alarm();
  } else if (entered == 2) {
    temp();
  } else if (entered == 3) {
    editTime();
  } else if (entered == 4) {
    delay(500); // Wait for enter buttom to change his duty.
    editDate();
  }
  display.display();
}

void setup() {
  pinMode(8, OUTPUT); // buzzer
  pinMode(9, OUTPUT); // lamp

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  delay(500);
}

void loop() {
  chAlarm();
  displayMenu();
  lamp();
}

void clock() {
  DateTime now = rtc.now();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 19);
  if (now.hour() < 10) {
    display.print('0');
  }
  display.print(now.hour(), DEC);
  display.print(':');
  if (now.minute() < 10) {
    display.print('0');
  }
  display.print(now.minute(), DEC);
  display.print(':');
  display.display();
  display.setCursor(70, 19);
  if (now.second() < 10) {
    display.print('0');
  }
  display.print(now.second(), DEC);
  display.display();
  delay(1000);
}
void date() {
  DateTime now = rtc.now();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(now.year(), DEC);
  display.print("/");
  display.print(now.month(), DEC);
  display.print("/");
  display.print(now.day(), DEC);
  display.print(" (");
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(") ");
  display.display();
}
void temp() {
  delay(100);
  display.clearDisplay();
  DHT.read22(dataPin);
  float t = DHT.temperature;
  float h = DHT.humidity;

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.println(t);

  display.setCursor(0, 36);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");
  display.display();
}
void editTime() {
  DateTime now = rtc.now();

  int enterHour = 0;
  int enterMin = 0;
  int enterSec = 0;

  int down = digitalRead(2);
  int up = digitalRead(3);

  int enter = digitalRead(4);
  int back = digitalRead(5);
  int reset = digitalRead(6);

  enterHour = now.hour();
  enterMin = now.minute();
  enterSec = now.second();

  while (back == LOW) {

    up = digitalRead(3);
    down = digitalRead(2);

    enter = digitalRead(4);
    back = digitalRead(5);
    reset = digitalRead(6);

    if (reset == HIGH) // Works like a reset buttom
    {
      enterHour = 1;
      enterMin = 0;
      enterSec = 0;
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    if (enterHour < 10) {
      display.print('0');
    }
    display.print(enterHour);
    display.print(':');
    if (enterMin < 10) {
      display.print('0');
    }
    display.print(enterMin);
    display.print(':');
    display.setCursor(70, 19);
    if (enterSec < 10) {
      display.print('0');
    }
    display.print(enterSec);
    display.display();

    if (down == HIGH) {
      enterHour++;
      enterSec = 0;
      if (enterHour == 24) {
        enterHour = 1;
      }
      delay(200); // Debounce delay
    }
    if (up == HIGH) {
      enterMin++;
      enterSec = 0;
      if (enterMin == 60) {
        enterMin = 0;
      }
      delay(200); // Debounce delay
    }
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), enterHour, enterMin, enterSec));
  }
}
void editDate() {
  DateTime now = rtc.now();

  int enterYear = 2020;
  int enterMonth = 1;
  int enterDay = 1;

  down = digitalRead(2);
  up = digitalRead(3);

  enter = digitalRead(4);
  back = digitalRead(5);
  reset = digitalRead(6);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(now.year(), DEC);
  display.print("/");
  display.print(now.month(), DEC);
  display.print("/");
  display.print(now.day(), DEC);
  display.print(" (");
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);
  display.print(") ");
  display.display();

  enterYear = now.year();
  enterMonth = now.month();
  enterDay = now.day();

  while (back == LOW) {
      
    if (reset == HIGH) // Works like a reset buttom
    {
      enterYear = 2020;
      enterMonth = 1;
      enterDay = 1;
    }

    down = digitalRead(2);
    up = digitalRead(3);

    enter = digitalRead(4);
    back = digitalRead(5);
    reset = digitalRead(6);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(enterYear);
    display.print("/");
    display.print(enterMonth);
    display.print("/");
    display.print(enterDay);
    display.print(" (");
    display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    display.print(") ");
    display.display();

    if (down == HIGH) {
      enterYear++;
      if (enterYear == 2031) {
        enterYear = 2020;
      }
      delay(200); // Debounce delay
    }
    if (up == HIGH) {
      enterMonth++;
      if (enterMonth == 13) {
        enterMonth = 1;
      }
      delay(200); // Debounce delay
    }
    if (enter == HIGH) {
      enterDay++;
      if (enterDay == 31) {
        enterDay = 1;
      }
      delay(200); // Debounce delay
    }
    rtc.adjust(DateTime(enterYear, enterMonth, enterDay, now.hour(), now.minute(), now.second()));
  }
}
void alarm() {

  int hour = 0;
  int min = 0;
  int sec = 0;

  up = digitalRead(2);
  down = digitalRead(3);

  enter = digitalRead(4);
  back = digitalRead(5);
  reset = digitalRead(6);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 19);

  while (back == LOW) 
  {
    if (reset == HIGH) // Works like a reset buttom
    {
      setAlarmH = 0;
      setAlarmM = 0;
      setAlarmS = 0;
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    if (setAlarmH < 10) {
      display.print('0');
    }
    display.print(setAlarmH);
    display.print(':');
    if (setAlarmM < 10) {
      display.print('0');
    }
    display.print(setAlarmM);
    display.print(':');
    display.setCursor(70, 19);
    if (setAlarmS < 10) {
      display.print('0');
    }
    display.print(setAlarmS);
    display.display();
    if (up == HIGH) {
      setAlarmH++;
      setAlarmS = 0;
      if (setAlarmH >= 26) {
        setAlarmH = 0;
      }
      delay(200); // Debounce delay
    }
    if (down == HIGH) {
      setAlarmM++;
      setAlarmS = 0;
      if (setAlarmM >= 60) {
        setAlarmM = 0;
      }
      delay(200); // Debounce delay
    }

    down = digitalRead(3);
    up = digitalRead(2);

    enter = digitalRead(4);
    back = digitalRead(5);
    reset = digitalRead(6);

    display.display();
  }
}
void chAlarm() {
  DateTime now = rtc.now();
  while (now.hour() == setAlarmH && now.minute() == setAlarmM && now.second() == setAlarmS)
  {
    delay(1000);
    digitalWrite(8, HIGH);
    delay(200);
    digitalWrite(8, LOW);
    if (reset == HIGH) {
      setAlarmH = 0;
      setAlarmM = 0;
      setAlarmS = 0;
      break;
    }
    reset = digitalRead(6);
  }
}
void lamp() {
  down = digitalRead(2);
  up = digitalRead(3);

  if (down == HIGH && up == HIGH) {
    digitalWrite(9, HIGH);
  }
  else if (enter == HIGH && back == HIGH) {
    digitalWrite(9, LOW);
  }
}
