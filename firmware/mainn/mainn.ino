#include "globals.h"

void setup() {
  Serial.begin(115200);
  pinMode(SYNC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN , INPUT_PULLUP);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Detonator");
  display.display();

  turnOffAllRelays();
}

void loop() {
  if (digitalRead(SYNC_BUTTON_PIN) == LOW) {
    selectDeviceName();          // Step 1: Choose device A-Z
    get_hash_from_phone();       // Step 2: Receive records
    printSequenceToSerial();     // Step 3: Debug output

    // Step 4: Display confirmation
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print("Device: ");
    display.println(selectedDeviceName);
    display.println(" ready to ignite");
    display.display();

    Serial.print("Selected device: ");
    Serial.println(selectedDeviceName);

    delay(3000); 
    ignite();// Optional pause before going back to idle
  }

   

}

