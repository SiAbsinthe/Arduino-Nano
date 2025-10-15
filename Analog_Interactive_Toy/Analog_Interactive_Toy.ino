/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/AnalogReadSerial/
*/
// === 腳位設定 ===
const int buttonPin = 2;    // 按鈕接腳
const int RledPin = 9;      // 紅色 LED 腳位 (PWM)
const int GledPin = 10;     // 綠色 LED 腳位 (PWM)
const int BledPin = 11;     // 藍色 LED 腳位 (不使用，但可擴充)

// === 狀態變數 ===
int mood = 0;                      // 當前心情值
const int neutralMood = 10;        // 初始心情值
int buttonState = 0;               // 目前按鈕狀態
bool ButtonPressed = false;        // 紀錄是否剛按過按鈕

// === 時間相關 ===
unsigned long touchedTimer = 0;    // 上一次按鈕的時間
unsigned long reducedTimer = 0;    // 上一次遞減的時間

const long untouchedInterval = 5000;  // 沒有互動多久後開始遞減 (ms)
const long reducedInterval = 1000;    // 每次遞減的間隔時間 (ms)

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  mood = neutralMood;  // 初始化心情
}

void loop() {
  // 顯示目前的心情狀態（以顏色呈現）
  showLEDState(mood);

  // 讀取按鈕狀態
  buttonState = digitalRead(buttonPin);

  // 檢查按鈕是否被按下
  if (buttonState == HIGH && !ButtonPressed) {
    mood += 1;
    if (mood > 20) mood = 20;  // 上限
    touchedTimer = millis();   // 更新上次互動時間
    ButtonPressed = true;
  }

  if (buttonState == LOW && ButtonPressed) {
    ButtonPressed = false;
  }

  // 超過一段時間沒互動就開始遞減心情
  unsigned long currentTimer = millis();
  if (currentTimer - touchedTimer >= untouchedInterval) {
    if (currentTimer - reducedTimer >= reducedInterval) {
      mood -= 1;
      if (mood < 0) mood = 0;  // 下限
      reducedTimer = currentTimer;
    }
  }
}

// === 顯示心情狀態的 LED 顏色 ===
// mood 越低 → 紅越多，越高 → 綠越多，中間為黃
void showLEDState(int moodValue) {
  // 限制 mood 在 0~20
  moodValue = constrain(moodValue, 0, 20);

  // 將 mood 映射到 0~255 的範圍，用於 PWM
  int greenValue = map(moodValue, 0, 20, 0, 255);
  int redValue   = 255 - greenValue;

  analogWrite(RledPin, redValue);
  analogWrite(GledPin, greenValue);
  analogWrite(BledPin, 0); // 不使用藍色，可擴充用藍色表示「暴怒」或「超快樂」
}