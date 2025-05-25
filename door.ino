#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// WiFi Ayarları
const char* ssid = "FiberHGW_TP0F56_2.4GHz";
const char* password = "NueVuuPq";


// Azure IoT Hub Bilgileri (Yeni)
const char* mqttServer = "senior-project-iothub.azure-devices.net";
const int mqttPort = 8883;
const char* deviceId = "Door1";
const char* mqttUsername = "senior-project-iothub.azure-devices.net/Door1/?api-version=2018-06-30";
const char* mqttPassword = "SharedAccessSignature sr=senior-project-iothub.azure-devices.net%2Fdevices%2FDoor1&sig=FtrPni87A%2By0sqksMXg7e58xHCn8QL7DeA7vy6dFCGU%3D&se=1779641184";

// RFID
#define SS_PIN D4
#define RST_PIN D3
MFRC522 rfid(SS_PIN, RST_PIN);

// Servo
Servo myServo;
bool isOpen = false;
bool autoClose = false; // ✅ YENİ: sadece RFID'den açıldığında true olacak
unsigned long doorOpenedAt = 0;
const unsigned long AUTO_CLOSE_TIME = 5000; // 5 saniye

// MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastStatusSent = 0;

void setup_wifi() {
  delay(100);
  Serial.print("WiFi bağlanıyor...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" WiFi bağlantısı başarılı.");
}

void openDoor(bool fromRFID) {
  myServo.write(90);
  isOpen = true;
  doorOpenedAt = millis();
  autoClose = fromRFID; // RFID'den geldiyse otomatik kapanacak
  Serial.println("🔓 Kapı Açıldı");
}

void closeDoor() {
  myServo.write(0);
  isOpen = false;
  autoClose = false;
  Serial.println("🔒 Kapı Kapandı");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String command = "";
  for (int i = 0; i < length; i++) command += (char)payload[i];
  command.toLowerCase();
  Serial.println("Azure'dan gelen komut: " + command);

  if (command == "on") openDoor(false); // IoT → autoClose = false
  else if (command == "off") closeDoor();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT Azure'a bağlanıyor...");
    if (client.connect(deviceId, mqttUsername, mqttPassword)) {
      Serial.println(" MQTT bağlantısı başarılı.");
  client.subscribe("devices/Door1/messages/devicebound/#");
    } else {
      Serial.print(" HATA -> Kod: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  myServo.attach(D1);
  myServo.write(0);

  setup_wifi();
  espClient.setInsecure();
client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("Sistem hazır: Azure + RFID + Selective AutoClose");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Status gönder
  if (millis() - lastStatusSent > 10000) {
    lastStatusSent = millis();
    String status = isOpen ? "acik" : "kapali";
    String payload = "{\"status\":\"" + status + "\"}";
client.publish("devices/Door1/messages/events/", payload.c_str());
    Serial.println("Durum gönderildi: " + payload);
  }

  // Sadece RFID'den açıldıysa auto-close uygula
  if (isOpen && autoClose && (millis() - doorOpenedAt >= AUTO_CLOSE_TIME)) {
    closeDoor();
  }

  // RFID kart kontrolü
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("Kart Okundu: " + uid);

     // ✅ Yetkili kartlar
  if (uid == "424fab0" || uid == "9c70b25") {
    if (!isOpen) openDoor(true); // RFID → autoClose = true
  } else {
    Serial.println("⛔ Yetkisiz kart");
  }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
