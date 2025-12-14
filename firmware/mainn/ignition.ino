#include "globals.h"
int cindex = 0;
#include <WiFi.h>
#include <esp_now.h>
String MY_DEVICE_ID;
byte outputState =0b0;

RF24 radio(16, 17);
const byte address[6] = "00001";


void broadcast(int indexValue) {
  uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&indexValue, sizeof(indexValue));

  if (result == ESP_OK) {
    Serial.printf("Broadcasted index: %d\n", indexValue);
  } else {
    Serial.println("Broadcast failed!");
  }
}

void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send Success" : "Send Fail");
}

void checkAndRunTask() {
  while (portTimeDict[cindex].letter == MY_DEVICE_ID) {
    Serial.printf("Running task for index %d\n", cindex);   
    setSlot(portTimeDict[cindex].number1,true);
    Serial.println("Slot set to on!");
    delay(portTimeDict[cindex].number2);
    cindex += 1;
  }
  broadcast(cindex);
}

void turnOffAllRelays() {
  outputState = 0b00000000;  // Clear all bits (set all outputs to LOW/OFF)
  updateShiftRegister();
  Serial.println("All relays turned OFF.");
}

void receiveCallback(const esp_now_recv_info_t *info, const uint8_t *data, int dataLen) {
  if (dataLen != sizeof(int)) {
    Serial.println("Invalid data length received.");
    return;
  }
x
  int receivedIndex;
  memcpy(&receivedIndex, data, sizeof(int));

  Serial.printf("Device %s received index: %d\n", MY_DEVICE_ID, receivedIndex);
  cindex = receivedIndex;


  if (portTimeDict[cindex].letter == MY_DEVICE_ID){
   checkAndRunTask(); 
  }
}

void setSlot(int slot, bool state) {
  if (state)
    outputState |= (1 << slot);  //setting the bit using bitwise OR
  else
    outputState &= ~(1 << slot);  //Clearing the bit using bitwise AND
  updateShiftRegister();
}

void updateShiftRegister() {
  digitalWrite(LATCH, LOW);
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, outputState);

  digitalWrite(LATCH, HIGH);  // Bit pattern is copied from shift register to storage register
}


void ignite(){
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   MY_DEVICE_ID=String(selectedDeviceName);

     if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }

    Serial.println("ESP-NOW Init Success");

  esp_now_register_send_cb(sentCallback);
  esp_now_register_recv_cb(receiveCallback);

  // Add broadcast peer
  esp_now_peer_info_t peerInfo = {};
  uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(broadcastAddress)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add broadcast peer");
    }
  }

  // If this device should start first
  if (MY_DEVICE_ID=="A") {
    
    Serial.println(MY_DEVICE_ID);    
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    while (true) {
      if (radio.available()) {
    int receivedIndex = -1;
    radio.read(&receivedIndex, sizeof(receivedIndex));
    Serial.print("Received: ");
    Serial.println(receivedIndex);

    if (receivedIndex == myFireworkIndex) {
      Serial.println("FIREWORK TRIGGERED!");
      checkAndRunTask();
      
      
    }
  }}
    
  }

}
