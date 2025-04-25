#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN 2
#define LED_TYPE WS2812B 
#define COLOR_ORDER GRB 
#define NUM_LEDS 256 
#define WIDTH 32
#define HEIGHT 8
CRGB leds[NUM_LEDS];



const char* ssid = "brisa-188473";
const char* password = "rmy8wceg";

WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);
NTPClient timeClient(ntpUDP, "time.google.com", (-3) * 3600);

String formattedDate;
String dayStamp;
String timeStamp;

bool char_0[5][3] = {
  { 1, 1, 1 },
  { 1, 0, 1 },
  { 1, 0, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 }
};

bool char_1[5][3] = {
  { 0, 1, 0 },
  { 1, 1, 0 },
  { 0, 1, 0 },
  { 0, 1, 0 },
  { 0, 1, 0 }
};

bool char_2[5][3] = {
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 1, 1, 1 },
  { 1, 0, 0 },
  { 1, 1, 1 }
};

bool char_3[5][3] = {
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 0, 1, 1 },
  { 0, 0, 1 },
  { 1, 1, 1 }
};

bool char_4[5][3] = {
  { 1, 0, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 0, 0, 1 }
};

bool char_5[5][3] = {
  { 1, 1, 1 },
  { 1, 0, 0 },
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 1, 1, 1 }
};

bool char_6[5][3] = {
  { 1, 1, 1 },
  { 1, 0, 0 },
  { 1, 1, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 }
};

bool char_7[5][3] = {
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 0, 0, 1 },
  { 0, 0, 1 },
  { 0, 0, 1 }
};

bool char_8[5][3] = {
  { 1, 1, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 }
};

bool char_9[5][3] = {
  { 1, 1, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 },
  { 0, 0, 1 },
  { 1, 1, 1 }
};

uint32_t matriz4x4[8][32] = { 0x0 };

uint32_t color = 0x00000f;

int startRow = 1;
int startCol = 1;

void insertCharInMatrix(int position, int characterIndex) {
  switch (position) {
    case 1:
      startRow = 1;
      startCol = 1;
      break;
    case 2:
      startRow = 1;
      startCol = 5;
      break;
    case 3:
      startRow = 1;
      startCol = 9;
      break;
    case 4:
      startRow = 1;
      startCol = 13;
      break;
  }

  Serial.println(characterIndex);

  const bool(*charactere)[3];

  switch (characterIndex) {
    case 0: charactere = char_0; break;
    case 1: charactere = char_1; break;
    case 2: charactere = char_2; break;
    case 3: charactere = char_3; break;
    case 4: charactere = char_4; break;
    case 5: charactere = char_5; break;
    case 6: charactere = char_6; break;
    case 7: charactere = char_7; break;
    case 8: charactere = char_8; break;
    case 9: charactere = char_9; break;
    default: return;
  }

  if (charactere == nullptr) return;

  //escreve no array
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 3; j++) {
      if ((startRow + i < 8) && (startCol + j < 32)) {
        matriz4x4[startRow + i][startCol + j] = charactere[i][j] * color;
      }
    }
  }

  FastLED.clear();

  // Enviar matriz
  for (int col = 0; col < WIDTH; col++) {
    for (int row = 0; row < HEIGHT; row++) {
      int actualRow = (col % 2 == 0) ? row : (HEIGHT - 1 - row);
      int ledIndex = col * HEIGHT + actualRow;

      uint32_t hexColor = matriz4x4[row][col];
      leds[ledIndex] = CRGB((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF);
    }
  }

  FastLED.show();
  return;
};

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  // timeClient.setTimeOffset((-3) * 3600);
  timeClient.setUpdateInterval(10000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Reconnecting...");
    WiFi.reconnect();
    delay(5000); 
    return;
  }
  while (!timeClient.update()) {
    Serial.print(".");
    timeClient.forceUpdate();
    delay(5000); 
  }
  formattedDate = timeClient.getFormattedTime();
  int hourTens = formattedDate[0] - '0';
  int hourUnits = formattedDate[1] - '0';
  int minuteTens = formattedDate[3] - '0';
  int minuteUnits = formattedDate[4] - '0';

  insertCharInMatrix(1, hourTens);
  insertCharInMatrix(2, hourUnits);
  insertCharInMatrix(3, minuteTens);
  insertCharInMatrix(4, minuteUnits);
  delay(10000);
}