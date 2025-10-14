/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/
// ===== HSV 轉 RGB 函式 =====
void HSVtoRGB(float h, float s, float v, int &r, int &g, int &b) {
  float c = v * s;
  float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
  float m = v - c;

  float r1, g1, b1;

  if (h < 60) {
    r1 = c; g1 = x; b1 = 0;
  } else if (h < 120) {
    r1 = x; g1 = c; b1 = 0;
  } else if (h < 180) {
    r1 = 0; g1 = c; b1 = x;
  } else if (h < 240) {
    r1 = 0; g1 = x; b1 = c;
  } else if (h < 300) {
    r1 = x; g1 = 0; b1 = c;
  } else {
    r1 = c; g1 = 0; b1 = x;
  }

  r = (r1 + m) * 255;
  g = (g1 + m) * 255;
  b = (b1 + m) * 255;
}

// ===== 七色快閃與固定顏色通用 RGB 設定函式 =====
void digitalColor(int color) {
  switch (color) {
    case 0: digitalWrite(3, HIGH); digitalWrite(4, HIGH); digitalWrite(5, HIGH); break; // 白
    case 1: digitalWrite(3, HIGH); digitalWrite(4, LOW);  digitalWrite(5, LOW);  break; // 紅
    case 2: digitalWrite(3, LOW);  digitalWrite(4, HIGH); digitalWrite(5, LOW);  break; // 綠
    case 3: digitalWrite(3, LOW);  digitalWrite(4, LOW);  digitalWrite(5, HIGH); break; // 藍
    case 4: digitalWrite(3, HIGH); digitalWrite(4, HIGH); digitalWrite(5, LOW);  break; // 黃
    case 5: digitalWrite(3, HIGH); digitalWrite(4, LOW);  digitalWrite(5, HIGH); break; // 品紅
    case 6: digitalWrite(3, LOW);  digitalWrite(4, HIGH); digitalWrite(5, HIGH); break; // 青
  }
}

// ===== 全域變數區 =====
const int buttonPin = 2;
const int RledPin = 3;
const int GledPin = 4;
const int BledPin = 5;

int mode = 0;               // 0 = HSV 呼吸, 1 = 快閃, 2 = 固定色
int lastButtonState = HIGH;
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 200;

int brightness = 0;
int fadeAmount = 5;
float hue = 0;

unsigned long prevMillis = 0;
const int flashInterval = 250;
int flashColor = 0;
bool flashOn = false;

int fixedColor = 0;

void setup() {
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(buttonPin);

  // === 模式切換邏輯（含固定色循環）===
  if (lastButtonState == HIGH && reading == LOW && (millis() - lastDebounce > debounceDelay)) {
    if (mode == 0) {
      mode = 1; // HSV → 快閃
    }
    else if (mode == 1) {
      mode = 2; // 快閃 → 固定色
      fixedColor = 0; // 從白開始
    }
    else if (mode == 2) {
      fixedColor++;
      if (fixedColor > 6) {
        fixedColor = 0;
        mode = 0; // 循環結束 → 回到 HSV 呼吸
      }
    }
    lastDebounce = millis();
  }

  lastButtonState = reading;

  // === 模式 0：HSV 呼吸 ===
  if (mode == 0) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }

    hue += 0.8;
    if (hue >= 360) hue = 0;

    int r, g, b;
    HSVtoRGB(hue, 1.0, brightness / 255.0, r, g, b);
    analogWrite(RledPin, r);
    analogWrite(GledPin, g);
    analogWrite(BledPin, b);

    delay(20);
  }

  // === 模式 1：七色快閃 ===
  else if (mode == 1) {
    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis >= flashInterval) {
      prevMillis = currentMillis;
      flashOn = !flashOn;

      if (flashOn) {
        digitalColor(flashColor);
      } else {
        digitalWrite(RledPin, LOW);
        digitalWrite(GledPin, LOW);
        digitalWrite(BledPin, LOW);
        flashColor++;
        if (flashColor > 6) flashColor = 0;
      }
    }
  }

  // === 模式 2：固定顏色 ===
  else if (mode == 2) {
    digitalColor(fixedColor);
  }
}