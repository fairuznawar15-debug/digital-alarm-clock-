# digital-alarm-clock-
An arduino project that will show time , date and also work as a timer, stopwatch . Later on, Maybe it will have global clock version . 
#Connections:
| Component   | Pin        | Arduino |
| ----------- | ---------- | ------- |
| OLED        | VCC        | 5V      |
| OLED        | GND        | GND     |
| OLED        | SDA        | A4      |
| OLED        | SCL        | A5      |
| Buzzer      | +          | D9      |
| Buzzer      | −          | GND     |
| MENU button | one side   | D2      |
| UP button   | one side   | D3      |
| DOWN button | one side   | D4      |
| OK button   | one side   | D5      |
| All buttons | other side | GND     |

#Code: For Simple Interface [ Time+Date+Day ] 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// ================= TIME =================

// Starting time: 11:49 PM
int hour = 11;
int minute = 23;
int second = 0;


// ================= DATE =================

// Starting date: Friday, 02-10-26
int day = 3;
int month = 10;
int year = 2026;

// 0 = Sunday
// 1 = Monday
// 2 = Tuesday
// 3 = Wednesday
// 4 = Thursday
// 5 = Friday
// 6 = Saturday
int dayOfWeek = 6;


unsigned long previousMillis = 0;


// ================= DAY NAMES =================

const char* days[] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};


// ================= DAYS IN MONTH =================

int daysInMonth(int m, int y) {

  // February
  if (m == 2) {

    // Leap year
    if ((y % 400 == 0) || (y % 4 == 0 && y % 100 != 0))
      return 29;

    else
      return 28;
  }

  // 30-day months
  if (m == 4 || m == 6 || m == 9 || m == 11)
    return 30;

  // 31-day months
  return 31;
}


// ================= SETUP =================

void setup() {

  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.display();
}


// ================= MAIN LOOP =================

void loop() {

  // Update every 1 second
  if (millis() - previousMillis >= 1000) {

    previousMillis += 1000;

    second++;


    // ---------- SECONDS ----------

    if (second >= 60) {

      second = 0;
      minute++;
    }


    // ---------- MINUTES ----------

    if (minute >= 60) {

      minute = 0;
      hour++;
    }


    // ---------- HOURS ----------

    if (hour >= 24) {

      hour = 0;

      day++;
      dayOfWeek++;


      // Saturday -> Sunday
      if (dayOfWeek > 6)
        dayOfWeek = 0;
    }


    // ---------- DATE ----------

    if (day > daysInMonth(month, year)) {

      day = 1;
      month++;
    }


    // ---------- MONTH ----------

    if (month > 12) {

      month = 1;
      year++;
    }


    // ================= OLED =================

    display.clearDisplay();


    // ---------- 12-HOUR TIME ----------

    int displayHour = hour % 12;

    if (displayHour == 0)
      displayHour = 12;


    // TIME: HH:MM:SS
    display.setTextSize(2);

    display.setCursor(4, 0);


    // Hour
    if (displayHour < 10)
      display.print("0");

    display.print(displayHour);

    display.print(":");


    // Minute
    if (minute < 10)
      display.print("0");

    display.print(minute);

    display.print(":");


    // Second
    if (second < 10)
      display.print("0");

    display.print(second);


    // ---------- AM / PM ----------

    display.setTextSize(1);

    display.setCursor(57, 20);

    if (hour < 12)
      display.print("AM");
    else
      display.print("PM");


    // ---------- DAY ----------

    display.setTextSize(1);

    display.setCursor(42, 35);

    display.print(days[dayOfWeek]);


    // ---------- DATE DD-MM-YY ----------

    display.setCursor(42, 51);

    // Day
    if (day < 10)
      display.print("0");

    display.print(day);

    display.print("-");


    // Month
    if (month < 10)
      display.print("0");

    display.print(month);

    display.print("-");


    // Year
    if ((year % 100) < 10)
      display.print("0");

    display.print(year % 100);


    // Send to OLED
    display.display();
  }
}
