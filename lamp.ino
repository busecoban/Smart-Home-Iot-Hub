#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// Wi-Fi Bilgileri
const char* ssid = "FiberHGW_TP0F56_2.4GHz";
const char* password = "NueVuuPq";



// Azure IoT Hub Bilgileri
const char* mqttServer = "senior-project-iothub.azure-devices.net";
const int mqttPort = 8883;
const char* deviceId = "Lamp1";
const char* mqttUsername = "senior-project-iothub.azure-devices.net/Lamp1/?api-version=2018-06-30";
const char* mqttPassword ="SharedAccessSignature sr=senior-project-iothub.azure-devices.net%2Fdevices%2FLamp1&sig=5xIWfQb6hS4qfg%2Bws17eWSJD14EAUNXoUHm7DG15680%3D&se=1779641129";


// Röle ve RGB LED Pinleri
#define RELAY1 D1
#define RED_PIN D5
#define GREEN_PIN D6
#define BLUE_PIN D7

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// HEX Rengi RGB'ye uygula
void applyHexColor(String hex) {
  if (hex.length() != 7 || hex.charAt(0) != '#') {
    Serial.println("⚠️ Geçersiz HEX formatı.");
    return;
  }

  long color = strtol(hex.substring(1).c_str(), NULL, 16);
  int red = (color >> 16) & 0xFF;
  int green = (color >> 8) & 0xFF;
  int blue = color & 0xFF;

  analogWrite(RED_PIN, 255 - red);    // Ortak anot için ters PWM
  analogWrite(GREEN_PIN, 255 - green);
  analogWrite(BLUE_PIN, 255 - blue);

  Serial.printf("🎨 Renk uygulandı: R=%d G=%d B=%d\n", red, green, blue);
}

// MQTT mesajlarını işleme
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();
  message.replace("\"", "");

  Serial.print("📩 Komut alındı: ");
  Serial.println(message);

  if (message == "ON") {
    digitalWrite(RELAY1, LOW);
    Serial.println("💡 Lamba AÇILDI");
  } else if (message == "OFF") {
    digitalWrite(RELAY1, HIGH);
    Serial.println("💡 Lamba KAPATILDI");
  } else if (message == "STATUS") {
    String status = (digitalRead(RELAY1) == LOW) ? "ON" : "OFF";
    String statusMessage = "{\"status\":\"" + status + "\"}";
    mqttClient.publish("devices/Lamp1/messages/events/", statusMessage.c_str());
    Serial.println("📤 Durum yayınlandı: " + statusMessage);
  } else if (message.startsWith("#")) {
    applyHexColor(message);
  } else {
    Serial.println("⚠️ Tanımsız komut.");
  }
}

// MQTT bağlantısını tekrar kur
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("🔄 MQTT bağlantısı deneniyor...");
    if (mqttClient.connect(deviceId, mqttUsername, mqttPassword)) {
      Serial.println("✅ MQTT bağlantısı başarılı!");
      mqttClient.subscribe("devices/Lamp1/messages/devicebound/#");
    } else {
      Serial.print("❌ Hata: ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

// Başlat
void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Başlatılıyor...");

  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, HIGH);  // Başlangıçta kapalı

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  analogWrite(RED_PIN, 255);    // Ortak anot LED için LED'i söndür
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);

  WiFi.begin(ssid, password);
  Serial.print("📡 WiFi bağlanıyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi bağlantısı kuruldu");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 100000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\n✅ Saat ayarlandı");

  secureClient.setInsecure(); // Sertifika doğrulama kapalı
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}

// Ana döngü
void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}
