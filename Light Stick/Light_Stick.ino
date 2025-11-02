/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/AnalogReadSerial/
*/
// === Arduino Nano：單一程式整合 ===
// 硬體：按鈕 D2 接 GND（INPUT_PULLUP），LED 接 D3（串 220Ω 到 GND）

// 腳位
const int buttonPin = 2;
const int ledPin    = 3;

const bool COMMON_ANODE = true;  // 共陽就 true，共陰就 false
#define LED_ON  (COMMON_ANODE ? LOW  : HIGH)
#define LED_OFF (COMMON_ANODE ? HIGH : LOW)

// 模式（App 層級）
enum AppMode { MODE_DEMO, MODE_FEATURE };
AppMode appMode = MODE_FEATURE;   // 預設進入作業需求的「功能模式」

// 功能模式下的子狀態（0=恆亮, 1=慢閃, 2=中閃, 3=快閃）
byte featureMode = 0;

// 閃爍間隔（ms）
const unsigned long SLOW_INTERVAL   = 800;  // 慢閃
const unsigned long MEDIUM_INTERVAL = 400;  // 不快不慢
const unsigned long FAST_INTERVAL   = 150;  // 快閃

// 防彈跳
const unsigned long DEBOUNCE_MS = 30;
int lastReading = HIGH;         // 最近一次讀取值
int stableBtn   = HIGH;         // 穩定後的按鈕值（INPUT_PULLUP：未按=HIGH，按下=LOW）
unsigned long lastDebounceTime = 0;

// 長按判斷
const unsigned long LONG_PRESS_MS = 800;
bool pressing = false;
bool longHandled = false;
unsigned long pressStart = 0;

// 閃爍控制
bool ledState = false;
unsigned long lastToggleTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // 初始（功能模式＋恆亮）
  appMode = MODE_FEATURE;
  featureMode = 0;
  digitalWrite(ledPin, LED_ON);   // << 使用 LED_ON
}

void loop() {
  // === 防彈跳讀取 ===
  int reading = digitalRead(buttonPin);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if (millis() - lastDebounceTime > DEBOUNCE_MS) {
    if (reading != stableBtn) {
      // 邊緣事件
      stableBtn = reading;
      if (stableBtn == LOW) {
        // 按下瞬間
        pressing = true;
        longHandled = false;
        pressStart = millis();
      } else {
        // 放開瞬間
        if (pressing) {
          unsigned long pressDur = millis() - pressStart;
          if (!longHandled && pressDur < LONG_PRESS_MS) {
            // 短按
            onShortPress();
          }
          pressing = false;
        }
      }
    }
  }
  lastReading = reading;

  // 長按偵測（按住且尚未處理）
  if (pressing && !longHandled) {
    if (millis() - pressStart >= LONG_PRESS_MS) {
      longHandled = true;
      toggleAppMode();  // 長按切換 DEMO / FEATURE
    }
  }

  // 依模式驅動 LED
  if (appMode == MODE_DEMO) {
    // DEMO：按下就亮、放開就滅
    digitalWrite(ledPin, (stableBtn == LOW) ? LED_ON : LED_OFF); 
  } else {
    // FEATURE：四態循環
    switch (featureMode) {
      case 0: // 恆亮
        digitalWrite(ledPin, LED_ON);   
        break;
      case 1: // 慢閃
        blinkIfInterval(SLOW_INTERVAL);
        break;
      case 2: // 中閃
        blinkIfInterval(MEDIUM_INTERVAL);
        break;
      case 3: // 快閃
        blinkIfInterval(FAST_INTERVAL);
        break;
    }
  }
}

// === 短按：功能模式下切換閃爍子狀態（恆亮 -> 慢閃 -> 中閃 -> 快閃 -> 恆亮）===
void onShortPress() {
  if (appMode == MODE_FEATURE) {
    featureMode = (featureMode + 1) % 4;
    if (featureMode == 0) {
      digitalWrite(ledPin, LED_ON);   
    } else {
      // 進入閃爍模式時重置
      ledState = true;
      lastToggleTime = millis();
      digitalWrite(ledPin, LED_ON);   
    }
  }
}

// === 長按：切換 DEMO / FEATURE，並以 LED 給予提示 ===
void toggleAppMode() {
  if (appMode == MODE_DEMO) {
    appMode = MODE_FEATURE;
    featureMode = 0;
    featureBlinkConfirm(2); // FEATURE：2 次快閃
    digitalWrite(ledPin, LED_ON);    
  } else {
    appMode = MODE_DEMO;
    featureBlinkConfirm(1); // DEMO：1 次長亮
  }
}

// 切換模式時的提示閃爍（阻塞式，時間很短，易看懂）
void featureBlinkConfirm(byte kind) {
  if (kind == 1) {
    // 1 次長亮
    digitalWrite(ledPin, LED_ON);  delay(300);   
    digitalWrite(ledPin, LED_OFF); delay(200);   
  } else {
    // 2 次快閃
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, LED_ON);  delay(120); 
      digitalWrite(ledPin, LED_OFF); delay(120); 
    }
  }
}

// 非阻塞閃爍
void blinkIfInterval(unsigned long intervalMs) {
  unsigned long now = millis();
  if (now - lastToggleTime >= intervalMs) {
    lastToggleTime = now;
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? LED_ON : LED_OFF);  
  }
}
