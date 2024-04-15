/* Project 999
  v1 - to run from a Lilygo T-Display Keyboard Module
Programmed by RC & MC. Original idea from TT
Last update 2024-04-12
*/


// Import required libraries

//#include <avr/pgmspace.h>
//#include <M5StickCPlus.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Keypad.h>
//#include "Button2.h"
//#include "esp_adc_cal.h"
//#include "bmp.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include "UTF8ToGB2312.h"  //conversion to Chinese GB so that the 16 glcd works (the rest is UTF-8)
#include "text.h"
#include "web.h"


#define BIGFONT &FreeMonoBold24pt7b
#define MIDBIGFONT &FreeMonoBold18pt7b
#define MIDFONT &FreeMonoBold12pt7b
#define SMALLFONT &FreeMonoBold9pt7b
#define TINYFONT &TomThumb

TFT_eSPI tft = TFT_eSPI();

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 21, 27, 26, 22 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 33, 32, 25 };      //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// Replace with your network credentials

const char *ssid = "LULU-999";
const char *password = "99999999";

unsigned long aliveInterval = 30000;
unsigned long lastInterval = 0;

bool ledState = 0;
const int ledPin = 2;

int phraseNumber = 0;
char phrase[60];


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0) {
      ledState = !ledState;
      ws.textAll(String(ledState));
      tft.setTextColor(TFT_YELLOW);
      tft.print("Toggle");
    }
  }
}

void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());

      tft.fillScreen(TFT_BLACK);  // Set BLACK to the background color.  将黑色设置为底色

      tft.setCursor(40, 0, 1);  // Set the cursor at (0,0) and the size to 1(mandatory).

      tft.println("Connected");
      ws.textAll("---");
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      
      tft.fillScreen(TFT_BLACK);  // Set BLACK to the background color.  将黑色设置为底色

      tft.setCursor(40, 0, 1);  // Set the cursor at (0,0) and the size to 1(mandatory).

      tft.println("WS disconnected");
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      digitalWrite(ledPin, ledState);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

String processor(const String &var) {
  if (var == "STATE") {
    return ledState ? "ON" : "OFF";
  }
  if (var == "CHECK") {
    return ledState ? "checked" : "";
  }
  if (var == "ALIVE") {
    return ledState ? "1" : "0";
  }
  return String();
}

void setup() {

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  //M5.begin();

  tft.begin();
  tft.setRotation(1);
  // Serial port for debugging purposes
  //.loadHzk16();  // Load the Chinese character library (be sure to load
  // before using the Chinese character library).
  // 加载汉字库(务必在使用汉字库前加载)
  tft.setTextColor(TFT_YELLOW,
                   TFT_BLACK);  // Set the text color to white and the text
                                // background color to black (mandatory).
                                // 设置文字颜色为白色,文字背景颜色为黑色(必加)
  // Set text with red highlight color
  //tft.setHighlightColor(
  //  TFT_RED);  // Set the text highlight color to red.  设置文字高亮颜色为红色
  tft.setTextSize(
    2);  // Set text size to 1.  设置字号大小为1 Set text size to 1(必加)

  Serial.begin(115200);

  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to Wi-Fi
  //WiFi.begin(ssid, password);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.print(".");
  // }


  tft.println("Log into WIFI: \n");  // Screen print string. 屏幕打印字符串
  tft.printf("%s \n\nAnd enter:\n\n", ssid);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Wait 100 ms for AP_START...");
  delay(100);

  Serial.println("Set softAPConfig");
  IPAddress Ip(192, 168, 1, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  tft.println(myIP);
  // Serial.println("");
  // Serial.println("Connected..!");
  Serial.print("Clients should connect to 192.168.1.1");
  //Serial.println(WiFi.localIP());

  ws.onEvent(eventHandler);
  server.addHandler(&ws);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  char key = keypad.getKey();

  if (key == '*') {  // If the button A is pressed.  如果按键 A 被按下

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(
      40, 0, 3);  // Set the cursor at (0,0) and the size to 1(mandatory).
                  // 将光标设置在(0,0)处,并设置字号为1(必加)
    tft.print("CLR");
    //ledState = !ledState;
    phraseNumber = 0;
    ws.textAll(" ");

  } else if (key == '#') {

    if (phraseNumber > 119) {

      tft.fillScreen(TFT_BLACK);
      tft.setCursor(
        40, 0, 1);  // Set the cursor at (0,0) and the size to 1(mandatory).
      phraseNumber = 0;
      tft.print("1 to 119");
      Serial.print(phraseNumber);
      Serial.println("BIG");
      // Serial.println(phrase);

    } else {
      strcpy_P(phrase, (char *)pgm_read_dword(&(phrases[phraseNumber])));
      phraseNumber = 0;

      Serial.print("Phrase n:");
      Serial.print(phraseNumber);
      Serial.print(" says ");
      Serial.println(phrase);
      ws.textAll(phrase);

      // Prepare the character array (the buffer)
      char tempGB[60];  //I make the temp a bit longer in case it comes a longer sentence
      for (int i = 0; i < 60; i++) {
        tempGB[i] = ' ';
      }

      strcpy(tempGB, GB.get(phrase).c_str());

      for (int j = 0; j < 3; j++) {
        char rowTemp[10];
        for (int i = 0; i < 10; i++) {
          rowTemp[i] = tempGB[j * 10 + i];
        }

        tft.fillScreen(TFT_BLACK);
        tft.setCursor(
          40, j * 40, 1);  // Set the cursor at (0,0) and the size to 1(mandatory).
                           // 将光标设置在(0,0)处,并设置字号为1(必加)
        //tft.writeHzk(rowTemp);
        // tft.println(rowTemp);
      }
    }


  } else if (key >= '0' && key <= '9') {

    if (phraseNumber == 0) {
      phraseNumber = key - '0';

    } else {
      phraseNumber = phraseNumber * 10;
      phraseNumber += key - '0';
    }

    tft.fillScreen(TFT_BLACK);  // Set BLACK to the background color.  将黑色设置为底色
    tft.setCursor(40, 0, 1);    // Set the cursor at (0,0) and the size to 1(mandatory).
    Serial.printf("Entered: #%u\n", phraseNumber);
    tft.print(phraseNumber);
  }
  //  else if (M5.BtnB.wasReleasefor(
  //              700)) {         // The button B is pressed for 700ms. 按键 B 按下
  //                              // 700ms,屏幕清空
  //   tft.fillScreen(TFT_BLACK);  // Set TFT_BLACK to the background color.  将黑色设置为底色
  //   tft.setCursor(0, 0);
  // }

  //send keep Alive signal from M5 to client browser
  if (millis() - lastInterval > aliveInterval) {
    lastInterval = millis();
    ws.textAll("ALIVE");
    Serial.print("Alive since: ");
    Serial.println(lastInterval);
  }
  ws.cleanupClients();
}