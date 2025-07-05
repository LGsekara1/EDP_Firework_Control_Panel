#include "globals.h"
char selectedDeviceName = 'A';

void selectDeviceName() {
  delay(1000);
  selectedDeviceName = 'A';
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Device: ");
  display.print(selectedDeviceName);
  display.display();

  unsigned long lastButtonTime = 0;

  while (true) {
    unsigned long now = millis();
    
    // Confirm selection on SYNC button press
    if (digitalRead(SYNC_BUTTON_PIN) == LOW) {
      delay(200); // debounce
      break;
    }

    if (now - lastButtonTime > 200) { // debounce
      if (digitalRead(BUTTON_UP) == LOW) {
        if (selectedDeviceName < 'Z') {
          selectedDeviceName++;
          display.clearDisplay();
          display.setCursor(10, 10);
          display.print("Device: ");
          display.print(selectedDeviceName);
          display.display();
        }
        lastButtonTime = now;
      }

      if (digitalRead(BUTTON_DOWN) == LOW) {
        if (selectedDeviceName > 'A') {
          selectedDeviceName--;
          display.clearDisplay();
          display.setCursor(10, 10);
          display.print("Device: ");
          display.print(selectedDeviceName);
          display.display();
        }
        lastButtonTime = now;
      }
    }
  }

  // Show final confirmation
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Selected: ");
  display.print(selectedDeviceName);
  display.display();
  delay(1500);
}