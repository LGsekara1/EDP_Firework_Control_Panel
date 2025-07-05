#include "globals.h"

// Display the records on OLED
void displayRecords() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int line = 0;
  int count = 0;

  for (const auto& [key, r] : portTimeDict) {
    if (count++ < scrollIndex) continue;  // Skip lines before scroll index

    String lineText = r.letter + ":" + String(r.number1) + "," + String(r.number2);
    display.setCursor(0, 6 + line * 10);
    display.println(lineText);
    line++;

    if (line >= linesPerScreen) break; // Only print visible lines
  }

  display.display();
}
 

// Handle POST from Flutter app
void handlePost() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
    return;
  }

  String body = server.arg("plain");
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON format\"}");
    return;
  }

portTimeDict.clear();  // Clear the dictionary first

JsonArray arr = doc.as<JsonArray>();
int index = 0;

for (JsonObject obj : arr) { 
  Record r;
  r.letter = obj["port"].as<String>();
  r.number1 = obj["port_no"];
  r.number2 = obj["time"];

  portTimeDict[index++] = r;  // Assign record to next index
}


  server.send(200, "application/json", "{\"status\":\"Records received successfully\"}");

  Serial.println("Received records:");
 for (const auto& [key, r] : portTimeDict) {
    Serial.printf("Letter: %s, Number 1: %d, Number 2: %d\n", r.letter.c_str(), r.number1, r.number2);
  }

  displayRecords();
}

// Receive hash values from phone via Wi-Fi AP
void get_hash_from_phone() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 6);
  display.println("connect phone");
  display.display();
  delay(2000);

  const char *ssid = "ESP32_Hotspot";
  const char *password = "";

  WiFi.softAP(ssid, password);
  Serial.println("ESP32 is now an Access Point");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/receive", HTTP_POST, handlePost);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(digitalRead(SYNC_BUTTON_PIN));

unsigned long lastButtonTime = 0;
while (digitalRead(SYNC_BUTTON_PIN) != LOW) {
  server.handleClient();

  // Handle scrolling buttons (debounced)
  unsigned long now = millis();
  if (now - lastButtonTime > 200) {
    if (digitalRead(BUTTON_UP) == LOW) {
      if (scrollIndex > 0) {
        scrollIndex--;
        displayRecords();
      }
      lastButtonTime = now;
    }

    if (digitalRead(BUTTON_DOWN) == LOW) {
      if (scrollIndex + linesPerScreen < portTimeDict.size()) {
        scrollIndex++;
        displayRecords();
      }
      lastButtonTime = now;
    }
  }
}

while (digitalRead(SYNC_BUTTON_PIN) != LOW) {

  
}


  server.stop();
  WiFi.softAPdisconnect(true);

  display.clearDisplay();
  display.setCursor(0, 6);
  display.println("Sync complete!");
  display.display();
  delay(2000);
}
