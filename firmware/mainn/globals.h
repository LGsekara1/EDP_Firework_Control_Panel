#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <map>  
#include <WiFi.h>
#include <esp_now.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Constants
#define SYNC_BUTTON_PIN 25
#define I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

int scrollIndex = 0;
const int linesPerScreen = 6;
extern char selectedDeviceName;

#define BUTTON_UP    33
#define BUTTON_DOWN  32

#define DATA_PIN  13
#define LATCH     27
#define CLK_PIN   14

// OLED and Server Instances
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

// Record structure
struct Record {
  String letter;
  int number1;
  int number2;
};



int myFireworkIndex = 3;

// Global record list (dictionary-style)
std::map<int, Record> portTimeDict;
