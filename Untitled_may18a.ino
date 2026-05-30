#include "arduino_secrets.h"
#include "thingProperties.h"
#include "DHT.h" // مكتبة حساس الحرارة

// --- تعريف أطراف التوصيل (Pins) - عدلها حسب بوردتك ---
#define DHTPIN 4          // بن حساس الحرارة (مثال: D4)
#define DHTTYPE DHT11     // أو DHT22 حسب الحساس اللي معاك
#define GAS_PIN 34        // بن حساس الغاز (Analog Pin)
#define LED_PIN 5         // بن الليدات
#define BUZZER_PIN 18     // بن البازر
#define FAN_ENA 19        // بن سرعة المروحة (يجب أن يدعم PWM)
#define FAN_IN1 21        // بن اتجاه المروحة 1 على الدرايفر
#define FAN_IN2 22        // بن اتجاه المروحة 2 على الدرايفر

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  delay(1500); 

  // --- 1. إعداد حالة الأطراف (Inputs / Outputs) ---
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_ENA, OUTPUT);
  pinMode(FAN_IN1, OUTPUT);
  pinMode(FAN_IN2, OUTPUT);
  
  // تحديد اتجاه دوران المروحة (شفط أو طرد)
  digitalWrite(FAN_IN1, HIGH);
  digitalWrite(FAN_IN2, LOW);

  // تشغيل حساس الحرارة
  dht.begin();

  // إعدادات الكلاود الافتراضية
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  
  // --- 2. قراءة الحساسات وتحديث الكلاود ---
  // سيتم تحديث المتغيرات تلقائياً على الداشبورد
  Temperature = dht.readTemperature();
  Gas_Level = analogRead(GAS_PIN);
  
  // --- 3. نظام الحماية الذكي المحلي (Edge Computing) ---
  // لو نسبة الغاز عدت حد الخطر (مثلاً 400 - عدلها حسب معايرة حساسك)
  if (Gas_Level > 400) {
    // تشغيل البازر فوراً
    digitalWrite(BUZZER_PIN, HIGH);
    Buzzer = true; // تحديث حالة زر البازر على الداشبورد ليكون ON
    
    // تشغيل المروحة بأقصى سرعة (255) لطرد الغاز
    analogWrite(FAN_ENA, 255);
    Fan_Control = 255; // تحديث قيمة شريط السرعة على الداشبورد
  } 
  
  // تأخير بسيط لمنع الضغط على الكلاود واستقرار القراءات
  delay(1000); 
}


/* --- دوال التحكم من الكلاود (تتنفذ عند تغيير القيمة من الداشبورد) --- */

void onLEDChange()  {
  // التحكم في الإضاءة
  if (LED == true) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void onBuzzerChange()  {
  // نتحكم في البازر يدوياً فقط لو مفيش خطر حقيقي
  if (Gas_Level <= 400) { 
    if (Buzzer == true) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

void onFanControlChange()  {
  // Fan_Control نوعها Float (تأكد إنها مربوطة بـ Slider من 0 لـ 255 في الداشبورد)
  // هنغير سرعة المروحة بناءً على قيمتها، لكن لو فيه غاز مش هنسمح بتبطيئها
  if (Gas_Level <= 400) {
    analogWrite(FAN_ENA, (int)Fan_Control); // تحويلها لـ Integer عشان تقبلها دالة PWM
  }
}

/* --- دوال القراءة (تترك فارغة لأننا نقرأها في دالة loop ولا نرسل لها أوامر) --- */
void onGasLevelChange()  {
}

void onTemperatureChange()  {
}