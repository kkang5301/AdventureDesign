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

// 조도센서 설정
const int LIGHT_SENSOR_PIN = A0;

// 랜덤 표정 배열
const char* randomEmojis[] = {"-_-", "^_^", "o_o", "O_O", "._.", "=_=", "T_T", ">_<", "~_~"};
const int numEmojis = 9;  // 랜덤 표정의 개수

// 랜덤 표정 변경을 위한 타이머 변수
unsigned long lastEmojiChangeTime = 0;
const unsigned long emojiChangePeriod = 500; // 5초

// 함수 프로토타입 선언
void displayEmoji(const char* emoji);
void displayRandomEmoji();
float getDistance(int trigPin, int echoPin);

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작
  
  lcd.init();       // LCD 초기화
  lcd.backlight();  // LCD 백라이트 켜기
  
  dht.begin();      // DHT 센서 시작
  
  Rtc.Begin();      // RTC 모듈 시작
  
  randomSeed(analogRead(0));  // 랜덤 시드 초기화
  
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  myServo.attach(8); // 서보 모터 핀 설정 (핀 7에 연결)
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

  // 조도 센서 처리
  if (lightValue >= 300) {
    displayEmoji(":)");
  } 
  // 습도 센서 처리
  else if (humidity >= 36) {
    displayEmoji(":(");
  } 
  // 온도 센서 처리
  else if (temperature >= 24.5) {
    displayEmoji(">_<");
  }
}

// LCD에 표정 표시 함수
void displayEmoji(const char* emoji) {
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print(emoji);
}

// 랜덤 표정 표시 함수
void displayRandomEmoji() {
  int randomIndex = random(numEmojis);
  displayEmoji(randomEmojis[randomIndex]);
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