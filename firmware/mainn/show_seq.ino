#include "globals.h"

void printSequenceToSerial() {
  Serial.println("Accessing sequence from another tab:");
  for (const auto& [key, r] : portTimeDict) {
    Serial.printf("Key: %d, Letter: %s, Number1: %d, Number2: %d\n", key, r.letter.c_str(), r.number1, r.number2);
  }
}
