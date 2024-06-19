#include "WiFiEsp.h"
#include <SoftwareSerial.h>

#define BUZZER_PIN 4
#define FLAME_DIGITAL_PIN 5
#define FLAME_ANALOG_PIN A2
#define LIGHT_DIGITAL_PIN 6
#define LIGHT_ANALOG_PIN A1
#define GAS_DIGITAL_PIN 7
#define GAS_ANALOG_PIN A0
#define TEMP_ANALOG_PIN A3
#define LED_LOW_PIN 11
#define LED_MIDDLE_PIN 12
#define LED_HIGH_PIN 13

struct SensorData {
  String name;
  int value;
  boolean state;
};

SoftwareSerial wifiSerial(2, 3);  // RX, TX
WiFiEspClient client;
char ssid[] = "moble_classroom1";
char pass[] = "12345678";
int status = WL_IDLE_STATUS;
char server[] = "192.168.0.205";

void setup() {
  //시리얼
  Serial.begin(115200);

  //와이파이
  wifiSerial.begin(9600);
  WiFi.init(&wifiSerial);

  //esp01 어댑터 연결 여부 체크
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true) {}
  }

  //와이파이 연결
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("You're connected to the network");
  printWifiStatus();

  // 핀 설정
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FLAME_DIGITAL_PIN, INPUT);
  pinMode(LED_LOW_PIN, OUTPUT);
  pinMode(LED_MIDDLE_PIN, OUTPUT);
  pinMode(LED_HIGH_PIN, OUTPUT);
}

void loop() {
  SensorData data[4];
  int cnt = getSensorValue(data);
  controlLED(cnt);
  bool tmp = false;
  for (int i = 0; i < 4; i++) {
    if (cnt > 1) {
      if (data[i].state == true) {
        httpRequest(data[i].name, data[i].value, data[i].state);
        tmp = true;
      }
    }
  }
  if (tmp == false) {
    httpRequest("false", 0, tmp);
  }
  delay(1000);
}

//http 요청
void httpRequest(String sensorType, int value, bool state) {
  Serial.println("Connecting...");

  //연결 초기화
  client.stop();

  delay(1000);

  //연결 성공 시
  if (client.connect(server, 8080)) {
    Serial.println("데이터 전송");
    String url;
    if(state == false){
      url = "POST /sensors/" + sensorType + "/save?value=" + String(value) + "&state=false HTTP/1.1";
    }
    else{
      url = "POST /sensors/" + sensorType + "/save?value=" + String(value) + "&state=true HTTP/1.1";
    }
    Serial.println(url);
    //http 요청을 함
    client.println(url);
    client.println(F("Host: 192.168.0.205:8080"));
    client.println("Connection: close");
    client.println();
  }
  //연결 실패 시
  else {
    Serial.println("데이터 전송 실패");
  }
}

//와이파이 정보 출력
void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

//센서 측정
int getSensorValue(SensorData data[]) {
  // 화재 센서
  data[0].name = "fire";
  data[0].value = digitalRead(FLAME_DIGITAL_PIN);

  //가스 센서
  data[1].name = "gas";
  data[1].value = analogRead(GAS_ANALOG_PIN);

  //조도 센서
  data[2].name = "light";
  data[2].value = analogRead(LIGHT_ANALOG_PIN);

  //온도 센서
  int sensorValue = analogRead(TEMP_ANALOG_PIN);      // TMP36 센서로부터 아날로그 값을 읽음
  float voltage = sensorValue * (5.0 / 1024.0);       // 아날로그 값을 전압으로 변환
  float temperatureC = (voltage - 0.5) * 100.0 - 35;  // TMP36의 온도 변환
  data[3].name = "temp";
  data[3].value = (int)temperatureC;
  Serial.println("불꽃 : " + String(data[0].value) + " 가스 : " + String(data[1].value) + " 조도 : " + String(data[2].value) + " 온도 : " + String(temperatureC));


  int sensorCount = 0;
  if (data[0].value == LOW) {
    data[0].state = true;
    Serial.print(" 불꽃 ");
    sensorCount++;
  }
  if (data[1].value > 150) {
    data[1].state = true;
    Serial.print(" 가스 ");
    sensorCount++;
  }
  if (data[2].value < 80) {
    data[2].state = true;
    Serial.println(" 조도 ");
    sensorCount++;
  }
  if (data[3].value > 30) {
    data[3].state = true;
    Serial.println(" 온도 ");
    sensorCount++;
  }
  return sensorCount;
}

void controlLED(int cnt) {
  if (cnt == 1) {
    digitalWrite(LED_LOW_PIN, HIGH);
  } else if (cnt == 2) {
    digitalWrite(LED_MIDDLE_PIN, HIGH);
    tone(BUZZER_PIN, 1000);  // 부저 주파수 설정
    delay(1000);             // 3초 동안 부저 울리기
    noTone(BUZZER_PIN);      // 부저 멈추기
  } else if (cnt >= 3) {
    digitalWrite(LED_HIGH_PIN, HIGH);
    tone(BUZZER_PIN, 1000);  // 부저 주파수 설정
    delay(800);              // 3초 동안 부저 울리기
    noTone(BUZZER_PIN);
  } else {
    digitalWrite(LED_HIGH_PIN, LOW);
    digitalWrite(LED_MIDDLE_PIN, LOW);
    digitalWrite(LED_LOW_PIN, LOW);
  }
}
