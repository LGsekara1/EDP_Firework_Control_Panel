//Meka hari eka device ekakata



#define DATA_PIN 23
#define CLK_PIN 22
#define LATCH 21

byte outputState = 0b00000000;


#include <map>
#include <Preferences.h>
//#include <GBALib_ShiftRegister.h>

//ShiftRegister sr(23, 22, 21);
Preferences prefs;

struct PortTime {
  String port;
  int port_no;
  unsigned long time;
};

std::map<int, PortTime> portTimeDict;

void savePortTimeData() {
  prefs.begin("fireworks", false);
  prefs.clear();

  for (const auto& entry : portTimeDict) {
    int key = entry.first;
    PortTime pt = entry.second;

    String baseKey = String(key);
    prefs.putString(("p" + baseKey).c_str(), pt.port);
    prefs.putUInt(("n" + baseKey).c_str(), pt.port_no);
    prefs.putULong(("t" + baseKey).c_str(), pt.time);
  }

  prefs.putUInt("count", portTimeDict.size());
  prefs.end();
}

void loadPortTimeData() {
  prefs.begin("fireworks", true);
  int count = prefs.getUInt("count", 0);

  for (int i = 0; i < count; i++) {
    String port = prefs.getString(("p" + String(i)).c_str(), "");
    int port_no = prefs.getUInt(("n" + String(i)).c_str(), 0);
    unsigned long time = prefs.getULong(("t" + String(i)).c_str(), 0);
    portTimeDict[i] = {port, port_no, time};
  }

  prefs.end();
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);




  Serial.begin(9600); 
  delay(5000);

  // Example defau,lt data:
  portTimeDict[0] = {"a", 0, 0};
  portTimeDict[1] = {"a", 7, 2000};
  portTimeDict[2] = {"a", 1, 0};
  portTimeDict[3] = {"a", 6, 2000};
  portTimeDict[4] = {"a", 2, 0};
  portTimeDict[5] = {"b", 2, 0};
  portTimeDict[6] = {"a", 5, 2000};
  portTimeDict[7] = {"a", 3, 0};
  portTimeDict[8] = {"a", 4, 2000};

  // savePortTimeData();
  // loadPortTimeData();

  for (const auto& entry : portTimeDict) {
    Serial.printf("Key %d -> Port: %s, Port No: %d, Time: %lu\n",
                  entry.first,
                  entry.second.port.c_str(),
                  entry.second.port_no,
                  entry.second.time);
      
      }
}

bool hiviru() {
  // Simulate a remote trigger
  return true;
}

void loop() {
  if (hiviru()) {
    for (const auto& entry : portTimeDict) {
      Serial.printf("Key %d -> Port: %s, Time: %lu\n", entry.first, entry.second.port.c_str(), entry.second.time);

      if (entry.second.port == "a") {
        Serial.println("a ekak");
        setSlot(entry.second.port_no,true);
      
      }

      delay(entry.second.time);
    }
    for (int i=0;i<8;i++){
      setSlot(i,false);
    }
    delay(5000);
    
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



