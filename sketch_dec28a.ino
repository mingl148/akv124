
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS1307.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

RtcDS1307<TwoWire> Rtc(Wire);


byte enc2 = 8;
byte enc3 = 3;

byte btn_ok = 2;

int filter_pin = 4;
int light_pin = 5;
int heater_pin = 6;
int airpump_pin = 7;

int led_state = 0;
int  display_num = 0;


volatile bool changed = false;
bool visible = true;
int set_time = 0;
int set_hour = 0;
int set_minute = 0;
int set_second = 0;
int set_temp = 25;
int *enc_change;
int *btn_change;
bool ledstate = false;

int *counter;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  Rtc.Begin();


  pinMode(enc2, INPUT_PULLUP);
  pinMode(enc3, INPUT_PULLUP);
  pinMode(btn_ok, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(filter_pin, OUTPUT);
  pinMode(light_pin, OUTPUT);
  pinMode(heater_pin, OUTPUT);
  pinMode(airpump_pin, OUTPUT);
  digitalWrite(filter_pin, HIGH);
  digitalWrite(light_pin, HIGH);
  digitalWrite(heater_pin, HIGH);
  digitalWrite(airpump_pin, HIGH);




  digitalWrite(LED_BUILTIN, LOW);
  attachInterrupt(digitalPinToInterrupt(btn_ok), button_ok, RISING);
  attachInterrupt(digitalPinToInterrupt(enc3), encoderChange, RISING);


  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      Serial.println(Rtc.LastError());
    }
    else
    {
    }
  }

  if (!Rtc.GetIsRunning())
  {
    Rtc.SetIsRunning(true);
  }

  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32

    while (1); // Don't proceed, loop forever)
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  for (int i = 0 ; i < 5; i++) {
    display.drawPixel(10, 10, SSD1306_WHITE);
    display.display();
    delay(500);
    display.clearDisplay();
    display.display();
    delay(500);
  }
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  int count = 0;
  while (set_time < 6) {
    btn_change = &set_time;
    switch (set_time) {
      case 0: {
          RtcDateTime now = Rtc.GetDateTime();
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(F("SET TIME"));
          display.setCursor(0, 16);
          printDateTime(now);
          display.display();
          delay(500);
          display.clearDisplay();
          display.display();
          delay(500);
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);
          ++count;
          break;
        }

      case 1: {
          RtcDateTime now = Rtc.GetDateTime();
          set_hour = now.Hour();
          set_minute = now.Minute();
          set_second = now.Second();
          set_time = 2;
          counter = &set_hour;
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);

        }

      case 2: {

          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(F("SET TIME"));
          display.setCursor(0, 16);
          if (visible) {
            if (set_hour < 10) {
              display.print(F("0"));
            }
            display.print(set_hour);
            visible = false;
          }
          else
          {
            display.print(F("  "));

            visible = true;
          }
          display.print(F(":"));
          if (set_minute < 10) {
            display.print(F("0"));
          }
          display.print(set_minute);
          display.print(F(":"));
          if (set_second < 10) {
            display.print(F("0"));
          }
          display.print(set_second);
          display.display();
          delay(500);
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);

          break;
        }
      case 3: {
          enc_change = &set_minute;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(F("SET TIME"));
          display.setCursor(0, 16);
          if (set_hour < 10) {
            display.print(F("0"));
          }
          display.print(set_hour);
          display.print(F(":"));
          if (visible) {
            if (set_minute < 10) {
              display.print(F("0"));
            }
            display.print(set_minute);

            visible = false;
          }
          else
          {
            display.print(F("  "));
            visible = true;
          }

          display.print(F(":"));
          if (set_second < 10) {
            display.print(F("0"));
          }
          display.print(set_second);
          display.display();
          delay(500);
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);
          break;
        }
      case 4: {
          enc_change = &set_second;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(F("SET TIME"));
          display.setCursor(0, 16);
          if (set_hour < 10) {
            display.print(F("0"));
          }
          display.print(set_hour);
          display.print(F(":"));

          if (set_minute < 10) {
            display.print(F("0"));
          }
          display.print(set_minute);
          display.print(F(":"));

          if (visible) {
            if (set_second < 10) {
              display.print(F("0"));
            }
            display.print(set_second);
            visible = false;
          }
          else
          {
            display.print(F("  "));
            visible = true;
          }
          display.display();
          delay(500);
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);
          break;
        }
      case 5: {
          enc_change = &set_temp;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print(F("SET TEMP"));
          display.setCursor(0, 16);
          display.print(set_temp);
          display.display();
          delay(500);
          ledstate = false;
          digitalWrite(LED_BUILTIN, ledstate);
          break;
        }
    }
    if (count > 20) {
      set_time = 25;
    }
  }
}




void loop() {
  // put your main code here, to run repeatedly:
  switch (display_num) {
    case 0: {
        RtcDateTime now = Rtc.GetDateTime();
        display.clearDisplay();
        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("TIME"));
        display.setCursor(0, 16);
        printDateTime(now);
        display_num = 1;

        break;
      }
    case 1: {
        sensors.requestTemperatures();
        display.clearDisplay();

        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("TEMP: "));
        display.setCursor(0, 16);
        display.print(sensors.getTempCByIndex(0));
        display_num = 2;

        break;
      }
    case 2: {
        display.clearDisplay();
        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("Filter: "));
        display.setCursor(0, 16);
        if (digitalRead(filter_pin) != false) {
          display.print("ON");
        }
        else {
          display.print("OFF");
        }
        display_num = 3;
        break;
      }
    case 3: {
        display.clearDisplay();
        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("HEATER: "));
        display.setCursor(0, 16);
        if (digitalRead(heater_pin) != false) {
          display.print("ON");
        }
        else {
          display.print("OFF");
        }
        display_num = 4;
        break;
      }
    case 4: {
        display.clearDisplay();
        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("Light: "));
        display.setCursor(0, 16);
        if (digitalRead(light_pin) != false) {
          display.print("ON");
        }
        else {
          display.print("OFF");
        }
        display_num = 5;
        break;
      }
    case 5: {
        display.clearDisplay();
        display.setCursor(0, 0);            // Start at top-left corner
        display.print(F("Airpump: "));
        display.setCursor(0, 16);
        if (digitalRead(airpump_pin) != false) {
          display.print("ON");
        }
        else {
          display.print("OFF");
        }
        display_num = 6;
        break;
      }

    default:
      display_num = 0;
      break;

  }
  display.display();
  delay(2000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void encoderChange() {
  detachInterrupt(digitalPinToInterrupt(enc3));
  byte state2 = digitalRead(enc2);
  if (changed == false) {
    if (state2 == LOW) {
      *counter = *counter + 1;
    }
    else {
      *counter = *counter - 1;
    }

    changed = true;
    ledstate = true;
    digitalWrite(LED_BUILTIN, ledstate);
  }
  attachInterrupt(digitalPinToInterrupt(enc3), encoderChange, RISING);
}

void button_ok() {
  detachInterrupt(digitalPinToInterrupt(btn_ok));
  delay(100);
  if (ledstate == false) {
    if (!digitalRead(btn_ok)) {
      *btn_change = *btn_change + 1;
      ledstate = true;
      digitalWrite(LED_BUILTIN, ledstate);
    }
  }
  attachInterrupt(digitalPinToInterrupt(btn_ok), button_ok, RISING);
}


void printDateTime(const RtcDateTime & dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u:%02u"),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );

  display.print(datestring);
}
