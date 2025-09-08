#include <Wire.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <SPIFFS.h>
#include <AnimatedGIF.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <ElegantOTA.h>
#include <MPU6050_tockn.h>
#include <Adafruit_NeoPixel.h>

#include "float_helper.hpp"
#include "Arduino.h"

// Display config

const int panelResX = 64;  // Number of pixels wide of each INDIVIDUAL panel module.
const int panelResY = 32;  // Number of pixels tall of each INDIVIDUAL panel module.
const int panel_chain = 2; // Total number of panels chained one to another.

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myYELLOW = dma_display->color565(255, 255, 0);
uint16_t myCYAN = dma_display->color565(0, 255, 255);
uint16_t myMAGENTA = dma_display->color565(255, 0, 255);
uint16_t myBLACK = dma_display->color565(0, 0, 0);

// Gifs

AnimatedGIF gif;
File gifFile;
static uint16_t gifPaletteRGB565[256];

// Wifi
const char *wifiName = "Proto";       // Number of pixels wide of each INDIVIDUAL panel module.
const char *wifiPass = "Test123456!"; // Number of pixels tall of each INDIVIDUAL panel module.

AsyncWebServer server(80);

// Fan
#define fanPWM 33 // PWM pin to control the fan
int fanDutyCycle = 100;

// Emotions
String currentEmotionGifName = "/neutral.gif";
String previousEmotionGifName = "/neutral.gif";

// Eccelerometer
MPU6050 *mpu6050;
FloatHelper float_helper;

// Tilt
String tiltUpEmotionGifName = "/happy.gif";
String tiltSideEmotionGifName = "/confused.gif";

bool tiltEnabled = true;
int tiltAnimationMaxDuration = 8000;

float tiltNeutralX = 0.3;
float tiltNeutralY = 0.92;
float tiltNeutralZ = 0.33;
float tiltSideX = 0.05;
float tiltSideY = 0.7;
float tiltSideZ = 0.73;
float tiltUpX = -0.17;
float tiltUpY = 0.99;
float tiltUpZ = 0.11;
float tiltTolerance = 0.1;

// NeoPixel
#define LEDS_PER_DISPLAY 24
#define DATA_PIN_EARS 32
int defaultBrightness = 80;
Adafruit_NeoPixel earLeds(LEDS_PER_DISPLAY, DATA_PIN_EARS, NEO_GRB + NEO_KHZ800);

//--------------------------------//WiFi server setup
void startWiFiWeb()
{
  WiFi.softAP(wifiName, wifiPass);

  server.on("/savefile", HTTP_POST, [](AsyncWebServerRequest *request) { // saves data from POST to file
    if (request->hasParam("file", true) && request->hasParam("content", true))
    {
      File file = SPIFFS.open("/anims/" + request->getParam("file", true)->value(), "w");
      if (!file)
      {
        Serial.println(F("[E] There was an error opening the file for saving an animation!"));
        file.close();
        request->send(400, "text/plain", F("Error opening file for writing!"));
      }
      else
      {
        Serial.println(F("[I] File saved!"));
        file.print(request->getParam("content", true)->value());
        file.close();
        request->send(200, "text/plain", F("File was saved."));
      }
    }
    else
    {
      request->send(400, "text/plain", F("No valid parameters detected!"));
    }
  });

  server.on("/file", HTTP_DELETE, [](AsyncWebServerRequest *request)
            { 
    if(request->hasParam("file")) {
      SPIFFS.remove("/anims/"+request->getParam("file")->value());
      request->send(200, "text/plain", F("File was deleted."));
    } else {
      request->send(400, "text/plain", F("Parameter 'file' not present!"));
    } });

  server.on("/emotion", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", currentEmotionGifName); });

  server.on("/emotion", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("name", true)) {
      previousEmotionGifName = currentEmotionGifName;
      currentEmotionGifName = String(request->getParam("name", true)->value());
      request->send(200, "text/plain", F("Emotion changed."));
    } else {
      request->send(400, "text/plain", F("No valid parameters detected!"));
    } });

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

  server.on("/fan", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(fanDutyCycle)); });

  server.on("/fan", HTTP_PUT, [](AsyncWebServerRequest *request)
            { 
               if(request->hasParam("duty", true)) {
                int duty = request->getParam("duty", true)->value().toInt();
                if(duty < 256 && duty >= 0) {
                  ledcWrite(0, duty);
                  fanDutyCycle = duty;
                  request->send(200, "text/plain", "Set PWM to: " + String(fanDutyCycle));
                  return;
                }
              }
              request->send(400, "text/plain", F("Invalid duty cycle"));
            });

  server.on("/gyro", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              if(!tiltEnabled){
                request->send(200, "text/plain", F("Tilt is disabled")); 
                return;
              }
              mpu6050->update();
              request->send(200, "text/plain", String(mpu6050->getAccX(), 2) + ";" + String(mpu6050->getAccY(), 2) + ";" + String(mpu6050->getAccZ(), 2)); 
            });

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not found"); });
  ElegantOTA.begin(&server);
  server.begin();

  ElegantOTA.setAutoReboot(true);
}

unsigned long lastMillsTilt = 0, tiltChange = 0;
bool wasTilt, tiltInitDone = false;

void processTilt()
{
  if (lastMillsTilt + 100 > millis() || !tiltEnabled)
  {
    return;
  }

  mpu6050->update();

  if (float_helper.isApproxEqual(mpu6050->getAccX(), mpu6050->getAccY(), mpu6050->getAccZ(), tiltUpX, tiltUpY, tiltUpZ, tiltTolerance) && !wasTilt)
  {
    Serial.println(F("[I] Tilt: UP!"));
    wasTilt = true;
    previousEmotionGifName = currentEmotionGifName;
    currentEmotionGifName = tiltUpEmotionGifName;
    tiltChange = millis();
  }
  else if (float_helper.isApproxEqual(mpu6050->getAccX(), mpu6050->getAccY(), mpu6050->getAccZ(), tiltSideX, tiltSideY, tiltSideZ, tiltTolerance) && !wasTilt)
  {
    Serial.println(F("[I] Tilt: Side!"));
    wasTilt = true;
    previousEmotionGifName = currentEmotionGifName;
    currentEmotionGifName = tiltSideEmotionGifName;
    tiltChange = millis();
  }
  else if ((tiltChange + tiltAnimationMaxDuration < millis() || float_helper.isApproxEqual(mpu6050->getAccX(), mpu6050->getAccY(), mpu6050->getAccZ(), tiltNeutralX, tiltNeutralY, tiltNeutralZ, tiltTolerance)) && wasTilt)
  {
    Serial.println(F("[I] Tilt: Neutral!"));
    wasTilt = false;
  }
  lastMillsTilt = millis();
}

void setupMatrices()
{
  HUB75_I2S_CFG mxconfig(
      panelResX,  // Module width
      panelResY,  // Module height
      panel_chain // Chain length
  );

  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();

  dma_display->fillScreen(myBLACK);
  dma_display->setCursor(5, 0);
  dma_display->setTextColor(myBLUE);
  dma_display->println("Startup...");
}

void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y, iWidth;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }
  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    pEnd = s + pDraw->iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < pDraw->iWidth)
    {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++)
        {
          dma_display->drawPixel(x + xOffset + pDraw->iX, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          iCount++;
        else
          s--;
      }
      if (iCount)
      {
        x += iCount; // skip these
        iCount = 0;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    for (x = 0; x < pDraw->iWidth; x++)
    {
      dma_display->drawPixel(x + pDraw->iX, y, usPalette[*s++]);
    }
  }
} /* GIFDraw() */

void *fileOpen(const char *filename, int32_t *pFileSize)
{
  gifFile = SPIFFS.open(filename, FILE_READ);
  if (!gifFile)
  {
    Serial.printf("Failed to open GIF file from SPIFFS: %s\n", filename);
    *pFileSize = 0;
    return NULL;
  }
  *pFileSize = gifFile.size();
  return (void *)&gifFile;
}

void fileClose(void *pHandle)
{
  (void)pHandle;
  if (gifFile)
    gifFile.close();
}

int32_t fileRead(GIFFILE *pHandle, uint8_t *pBuf, int32_t iLen)
{
  (void)pHandle;
  if (!gifFile)
    return 0;
  int32_t r = gifFile.read(pBuf, (size_t)iLen);
  return r;
}

int32_t fileSeek(GIFFILE *pHandle, int32_t iPosition)
{
  (void)pHandle;
  if (!gifFile)
    return -1;
  if (iPosition < 0)
    iPosition = 0;
  if (iPosition > (int32_t)gifFile.size())
    iPosition = gifFile.size();
  gifFile.seek(iPosition, SeekSet);
  return iPosition;
}

void listSPIFFS()
{
  Serial.println("SPIFFS contents:");
  File root = SPIFFS.open("/");
  if (!root)
  {
    Serial.println("Failed to open root directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Root is not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    Serial.printf("  %s  (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }
  Serial.println("---- End of SPIFFS listing ----");
}

void setupTilt()
{
  Wire.begin(SDA, SCL, 400000);
  Wire.beginTransmission (0x68);
  if (Wire.endTransmission () == 0) {
    tiltEnabled = true;
    mpu6050 = new MPU6050(Wire);
    mpu6050->begin();
  } else{
    tiltEnabled = false;
    Serial.println("MPU6050 not found. Tilt disabled");
  }
}

void setupFan()
{
  pinMode(fanPWM, OUTPUT);

  ledcSetup(0, 25000, 8);
  ledcAttachPin(fanPWM, 0);
  ledcWrite(0, fanDutyCycle);
}

void setup()
{
  Serial.begin(115200);

  setupTilt();
  setupMatrices();
  setupFan();

  if(!earLeds.begin()){
    Serial.println("Starting LED driver failed");
  }
  earLeds.setBrightness(defaultBrightness);
  
  if (!SPIFFS.begin(true))
  { // use true to format if needed
    Serial.println("SPIFFS mount failed!");
    while (true)
      delay(1000);
  }
  Serial.println("SPIFFS mounted.");

  gif.begin(LITTLE_ENDIAN_PIXELS);

  listSPIFFS();
  startWiFiWeb();

  Serial.println("Init done");
}

void loop()
{
  ElegantOTA.loop();
  processTilt();

  if (gif.open(currentEmotionGifName.c_str(), fileOpen, fileClose, fileRead, fileSeek, GIFDraw))
  {
    //Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    gif.playFrame(true, NULL);
    gif.close();
  }

  for (int8_t index = 0; index < LEDS_PER_DISPLAY; index++)
  {
    earLeds.setPixelColor(index, Adafruit_NeoPixel::Color(1,1,1));
  }
  earLeds.show(); 
}