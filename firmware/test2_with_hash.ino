#include <WiFi.h>
#include <esp_now.h>

// Define LED pins
#define RED_LED 25
#define YELLOW_LED 26
#define BLUE_LED 27
#define GREEN_LED 14

#define MY_DEVICE_ID 1  // Change this for each device
#define NUM_DEVICES 3

int cindex = 0;

// Send callback
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send Success" : "Send Fail");
}

// ✅ Updated receive callback for ESP32 core v3+
void receiveCallback(const esp_now_recv_info_t *info, const uint8_t *data, int dataLen) {
  if (dataLen != sizeof(int)) {
    Serial.println("Invalid data length received.");
    return;
  }

  int receivedIndex;
  memcpy(&receivedIndex, data, sizeof(int));

  Serial.printf("Device %d received index: %d\n", MY_DEVICE_ID, receivedIndex);
  cindex = receivedIndex;

  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  if (receivedIndex % NUM_DEVICES == MY_DEVICE_ID) {
    digitalWrite(YELLOW_LED, HIGH);
    delay(2000);
    digitalWrite(YELLOW_LED, LOW);

    int newIndex = receivedIndex + 1;
    broadcast(newIndex);
  }
}

void broadcast(int indexValue) {
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&indexValue, sizeof(indexValue));

  if (result == ESP_OK) {
    digitalWrite(GREEN_LED, HIGH);
    Serial.printf("Broadcasted index: %d\n", indexValue);
  } else {
    digitalWrite(BLUE_LED, HIGH);
    Serial.println("Broadcast failed!");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }

  Serial.println("ESP-NOW Init Success");

  esp_now_register_send_cb(sentCallback);
  esp_now_register_recv_cb(receiveCallback);  // ✅ uses updated signature

  // Add broadcast peer
  esp_now_peer_info_t peerInfo = {};
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(broadcastAddress)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add broadcast peer");
    }
  }

  if (MY_DEVICE_ID == 0) {
    delay(1000);
    broadcast(0);
  }
}

void loop() {
  digitalWrite(RED_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, LOW);
  delay(4900);
}
