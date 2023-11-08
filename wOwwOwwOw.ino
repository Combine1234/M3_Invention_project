#include "I2Cdev.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>

Adafruit_MPU6050 mpu;

char server[] = "monkey-mysql.mysql.database.azure.com";
char user[] = "RTM";
char password[] = "Monkey12345";
char db[] = "monkey";

WiFiClient client;
MySQL_Connection conn((Client *)&client);

int buzzer = 23;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;
char rd;
int prevVal;
int led = 13;
int pin11 = 11, pin10 = 10;
int val1, val2;
int valgy1 = 0, valgy2 = 0;
int16_t previousAngle = 90;
unsigned long previousTime = 0;

const char* ssid = "@SPR-WIFI-Teacher";
const char* password1 = "@spr2603";

void setup() {
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Wire.begin();
  Serial.println("Initialize MPU");
  Serial.begin(115200);
  WiFi.begin(ssid, password1);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
  }
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  // Resolve the MySQL server's hostname to an IP address
  IPAddress serverIP;
  WiFi.hostByName(server, serverIP);


  if (conn.connect(serverIP, 3306, user, password, db)) {
    Serial.println("MySQL connection successful");
  // Proceed with database operations
  } else {
    Serial.println("Connection to MySQL server failed.");
  // Handle the connection failure, possibly by retrying or reporting an error.
  }

}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  valx = a.acceleration.x;
  Serial.println(valx);
  unsigned long currentTime = millis();
  digitalWrite(buzzer, HIGH);
  if (abs(valx - previousAngle) > 8) {
    if ((valx >= 7 && valx <= 10) || (valx >= -7 && valx <= -10)) {
      if (currentTime - previousTime < 3000) {
        
        Serial.println("Yes");
        char INSERT_SQL[] = "INSERT INTO esp32 (Status_Report) VALUES ('มีการหกล้ม')";
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        cur_mem->execute(INSERT_SQL);
        delete cur_mem;
        digitalWrite(buzzer, LOW);
        delay(5000);
      }
    } else {
      Serial.println("No");
    }
    previousTime = currentTime;
    previousAngle = valx;
  }

  if (currentTime - previousTime > 3000) {
    previousAngle = 90;
  }
}
