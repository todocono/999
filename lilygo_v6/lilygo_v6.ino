/* Project 999
  v2a - to run from a Lilygo T-Display Keyboard Module
Programmed by RC & MC. Original idea from TT
Last update 2024-05-02
*/


// Import required libraries
#include <FS.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Keypad.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include "UTF8ToGB2312.h"  //conversion to Chinese GB so that the 16 glcd works (the rest is UTF-8) (needed for M5)

//Import font
#include "wqyMicroHei16.h"
//#include "NotoSansCJKscRegular16.h"


// Replace with network credentials
//according to the module selected (A = 4Mb)
// The SSID of the wifi will be picked from the first line of the config.csv file
const char *password = "99999999";  //the password is always the same

// Import phrases and webApp
// #include "web.h"
#include "sd_aux.h"

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 17
#define SD_CS 13


//Display settings
TFT_eSPI tft = TFT_eSPI();
uint16_t bg = TFT_BLACK;
uint16_t fg = TFT_WHITE;
//Keypad settings
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

unsigned long aliveInterval = 30000;
unsigned long lastInterval = 0;

bool ledState = 0;     //not used
const int ledPin = 2;  //not used

int phraseNumber = 0;
int numberOfPhrases = 0;

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
      tft.setCursor(40, 0, 1);    // Set the cursor at (0,0) and the size to 1(mandatory).

      tft.println("Connected");
      ws.textAll("---");
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());

      tft.fillScreen(TFT_BLACK);  // Set BLACK to the background color.  将黑色设置为底色
      tft.setCursor(40, 0, 1);    // Set the cursor at (0,0) and the size to 1(mandatory).
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
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  tft.begin();
  tft.setRotation(1);  // portrait
  tft.loadFont(wqyMicroHei16);
  //tft.loadFont(wqyMicroHei24);
  //tft.loadFont(NotoSansCJKscRegular16);

  fg = TFT_YELLOW;
  bg = TFT_BLACK;
  tft.setTextColor(fg, bg);
  tft.fillScreen(TFT_BLACK);

  pinMode(SD_MISO, INPUT_PULLUP);
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);


  //     SPI.begin(18, 36, 26, SD_CS_PIN);
  SPI.setDataMode(SPI_MODE0);
  SD.begin(SD_CS);
  delay(100);


  Serial.println("Reading SD...");
  tft.println("Reading SD...");

  if (!SD.begin(SD_CS)) {
    Serial.println("SDCard MOUNT FAIL");
  } else {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
  }


  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  delay(1000);

  String s = readFileLine(SD, "/config.csv", 1);  //.toCharArray(ssid, sizeof(ssid));
  Serial.print("would like to be Starting WiFi at ");
  Serial.println(s);
  Serial.print("which is: ");
  char ssid[s.length() + 1];

  s.toCharArray(ssid, s.length() + 1);
  //s.toCharArray(ssid, sizeof(ssid));
  Serial.println(ssid);
  tft.print("would like to be Starting WiFi at ");
  tft.println(ssid);
  delay(1000);

  File file = SD.open("/phrases.csv");
  String line = "";
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read till the end of the file...");
  int i = 0;
  while (file.available()) {
    // Serial.write(file.read());
    line = file.readStringUntil('\n');
    i++;
    Serial.print("phrase #");
    Serial.print(i);
    Serial.print(" is ");
    Serial.println(line);
  }
  tft.print(i);
  tft.println(" phrases read");
  numberOfPhrases = i;

  file.close();


  delay(1000);

  tft.fillScreen(bg);
  tft.setCursor(0, 0);
  //tft.println("CLR");
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

  // Route for root / web page (using the program memory)
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", index_html, processor);
  // });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SD, "/index.htm", "text/html");
  });

  server.serveStatic("/", SD, "/");

  // Start server
  server.begin();
}

void loop() {
  char key = keypad.getKey();

  if (key == '*') {  // If the button A is pressed.  如果按键 A 被按下
    tft.setCursor(0, 0);
    tft.fillScreen(TFT_BLACK);

    phraseNumber = 0;
    ws.textAll(" ");

  } else if (key == '#') {
    if ((phraseNumber > numberOfPhrases) || (phraseNumber <= 0)) {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(40, 0, 1);  // Set the cursor at (0,0) and the size to 1(mandatory).
      phraseNumber = 0;
      tft.print("1 to ");
      tft.println(numberOfPhrases);
      Serial.print(phraseNumber);
      Serial.print("1 to ");
      Serial.println(numberOfPhrases);

    } else {
      //fill in phrase with null characters
      // for (int i = 0; i < 60; i++) {
      //   phrase[i] = 0;
      // }

      //This used to be read from the program memory, now we read from SD
      // strcpy_P(phrase, (char *)pgm_read_dword(&(phrases[phraseNumber])));
      String s = readFileLine(SD, "/phrases.csv", phraseNumber);


      Serial.print("Phrase n: ");
      Serial.print(phraseNumber);
      Serial.print(" says ");
      Serial.println(s);
      ws.textAll(s);

      // Prepare the character array (the buffer)
      // char tempGB[60];  //I make the temp a bit longer in case it comes a longer sentence
      // for (int i = 0; i < 60; i++) {
      //   tempGB[i] = ' ';
      // }

      // strcpy(tempGB, GB.get(phrase).c_str());
      // strcpy(tempGB, phrase);

      tft.fillScreen(TFT_BLACK);
      // for (int j = 0; j < 2; j++) {
      //   //char rowTemp[10];


      //   tft.setCursor(40, j * 25 + 20, 3);  // Set the cursor at (0,0)

      //   for (int i = 0; i < 30; i++) {
      //     char c = phrase[j * 30 + i];
      //     if (c > 0) {
      //       tft.print(c);  //rowTemp[i] = tempGB[j * 10 + i];
      //     }
      //   }
      //   tft.println();
      // }
      tft.println(s);

      // record the phraseNumber in stats file (in order!)
      //const char *msg = phraseNumber + '\n';
      //char msg = {'0', '0', '0', '\n'};

      char buffer[10];  // Buffer to hold the converted string
      itoa(phraseNumber, buffer, 10);  // Convert integer to string with base 10
      const char* msg = buffer;

      appendFile(SD, "/stats.csv", msg);
      // put phraseNumber there
      phraseNumber = 0;
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


  //send keep Alive signal from M5 to client browser
  if (millis() - lastInterval > aliveInterval) {
    lastInterval = millis();
    ws.textAll("ALIVE");
    Serial.print("Alive since: ");
    Serial.println(lastInterval);
  }
  ws.cleanupClients();
}