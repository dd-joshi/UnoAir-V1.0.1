#include <Arduino.h>

// ---------------- Pins ----------------
const int POT_PIN    = A0;
const int SENSOR_PIN = 7;       // PWM from PM sensor
const int RED_LED    = 5;
const int GREEN_LED  = 6;
const int FAN_PWM    = 10;      // Timer1 OCR1B

// ---------------- Globals ----------------
int sensorSpeed = 0;
int potSpeed = 0;
int finalSpeed = 0;

// ---------------- Timer1 25kHz Setup ----------------
void setupPWM25kHz() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  
  // Mode 14: Fast PWM, TOP = ICR1
  // Only enable OCR1B (pin 10) for fan control
  TCCR1A = _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);  // Added WGM12!
  
  ICR1 = 639;  // 16MHz / 640 = 25kHz (was 320, which gave 50kHz)
  
  pinMode(10, OUTPUT);  // Fan PWM output
}

void analogWrite25k(int value) {
  value = constrain(value, 0, 639);  // Updated from 320
  OCR1B = value;
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  setupPWM25kHz();
  
  Serial.println("Air Purifier Ready");
}

// ---------------- Read Sensor PWM ----------------
float readSensorDuty() {
  unsigned long highTime = pulseIn(SENSOR_PIN, HIGH, 1100000);  // 1.1s timeout
  unsigned long lowTime  = pulseIn(SENSOR_PIN, LOW, 1100000);
  
  if (highTime == 0 || lowTime == 0) {
    return -1.0;  // Signal error
  }
  
  float duty = (highTime * 100.0) / (highTime + lowTime);
  return duty;
}

// ---------------- Main Loop ----------------
void loop() {
  // ----- 1. Read Pot (manual ceiling) -----
  int potRaw = analogRead(POT_PIN);
  potSpeed = map(potRaw, 0, 1023, 0, 639);  // Updated from 320
  
  // ----- 2. Read Sensor Duty & Set Fan Speed -----
  float duty = readSensorDuty();
  
  if (duty >= 0) {
    // Map duty cycle to fan speed based on air quality
    // Duty ranges from PM2.5 sensor (typical 0-15% for 0-500 µg/m³)
    
    if (duty <= 4.0) {
      // Good air quality (0-4%)
      sensorSpeed = 40 + (int)((duty / 4.0) * 120);  // 40-160
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
    }
    else if (duty <= 8.0) {
      // Moderate air quality (4-8%)
      sensorSpeed = 160 + (int)(((duty - 4.0) / 4.0) * 160);  // 160-320
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
    }
    else if (duty <= 12.0) {
      // Poor air quality (8-12%)
      sensorSpeed = 320 + (int)(((duty - 8.0) / 4.0) * 200);  // 320-520
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    else {
      // Hazardous air quality (>12%)
      sensorSpeed = 639;  // Full blast
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    
    sensorSpeed = constrain(sensorSpeed, 0, 639);
    
  } else {
    // Sensor timeout - fallback to manual control
    Serial.println("Sensor read error - using manual control");
    sensorSpeed = potSpeed;
  }
  
  // ----- 3. Override Logic (pot acts as ceiling) -----
  finalSpeed = min(sensorSpeed, potSpeed);
  
  // ----- 4. Apply minimum speed or turn off -----
  // Most fans stall below ~15% duty cycle
  if (finalSpeed > 0 && finalSpeed < 96) {  // 96 ≈ 15% of 639
    finalSpeed = 96;
  }
  
  // ----- 5. Apply PWM -----
  analogWrite25k(finalSpeed);
  
  // ----- Debug -----
  Serial.print("Duty: ");
  Serial.print(duty, 1);
  Serial.print("% | Sensor: ");
  Serial.print(sensorSpeed);
  Serial.print(" | Pot: ");
  Serial.print(potSpeed);
  Serial.print(" | Final: ");
  Serial.println(finalSpeed);
  
  delay(300);
}

