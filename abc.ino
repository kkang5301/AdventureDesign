#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <NewPing.h>
#include <Servo.h>

// LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT11 온습도 센서 설정
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 초음파 센서 설정
const int trigPin1 = 3; // 첫 번째 초음파 센서의 Trig 핀
const int echoPin1 = 4; // 첫 번째 초음파 센서의 Echo 핀
const int trigPin2 = 5; // 두 번째 초음파 센서의 Trig 핀
const int echoPin2 = 6; // 두 번째 초음파 센서의 Echo 핀

// 서보 모터 설정
Servo myServo;

// 조도센서 및 부저 설정
const int LIGHT_SENSOR_PIN = A0;

//부저
const int BUZZER_PIN = 7;

float getDistance(int trigPin, int echoPin);

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작
  
  lcd.init();       // LCD 초기화
  lcd.backlight();  // LCD 백라이트 켜기
  
  dht.begin();      // DHT 센서 시작
  
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  pinMode(BUZZER_PIN, OUTPUT); // 부저 핀 출력 모드 설정
  myServo.attach(8);           // 서보 모터 핀 설정 (핀 8에 연결)
}

void loop() {
  lcd.display();  // LCD 켜기
  
  // 센서 값 읽기
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  float distance1 = getDistance(trigPin1, echoPin1);
  float distance2 = getDistance(trigPin2, echoPin2);

  // 첫 번째 센서의 조건: 거리 5cm 이하 시 -60도로 회전
  if (distance1 <= 5) {
    myServo.write(120); // -60도 회전 (SG90은 0~180도 사이로 설정하므로 120도로 설정)
    delay(1000); // 안정적인 동작을 위한 지연
  }

  // 두 번째 센서의 조건: 거리 5cm 이하 시 60도로 회전
  if (distance2 <= 5) {
    myServo.write(60); // 60도 회전
    delay(1000); // 안정적인 동작을 위한 지연
  }

  if (lightValue >= 300) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Smile :)");
      buzz(1, 1000); // 부저 1회, 1초 간격
  } else if (humidity >= 36) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Sad :(");
      buzz(1, 1000); // 부저 1회, 1초 간격
  } else if (temperature >= 24.5) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Hot :/");
      buzz(1, 1000); // 부저 1회, 1초 간격
  } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("normal");
  }
}

// 초음파 센서 거리 측정 함수
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.0343) / 2; // 거리 계산 (단위: cm)
  
  return distance;
}

// 부저 작동 함수
void buzz(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500); // 부저 울림 시간
    digitalWrite(BUZZER_PIN, LOW);
    delay(delayTime - 500); // 나머지 대기 시간
  }
}