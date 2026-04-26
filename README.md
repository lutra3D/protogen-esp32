# protogen-esp32

Firmware for an **ESP32-Trinity-based Protogen** build with:
- animated HUB75 face display output,
- configurable RGB ear LEDs,
- fan speed control,
- IMU tilt telemetry,
- local web API + static web UI,
- BLE remote control,
- LittleFS-backed animation + settings storage,
- OTA update support.

## Hardware/firmware architecture

The firmware composes several controllers and managers:
- `FaceDisplay` renders animation files to a chained 64x32 HUB75 matrix setup.
- `EarController` drives ear LEDs (NeoPixel-compatible strip/ring).
- `FanController` controls fan speed through PWM.
- `TiltController` reads motion/tilt data over I2C.
- `WebServerManager` serves the API and static web assets over Wi‑Fi AP mode.
- `BLEController` exposes a BLE service/characteristic for remote commands.
- `SettingsStorage` persists runtime-adjustable settings.

See `src/main.cpp` for system wiring and startup order.

## Capabilities

### 1) Emotion playback and management
- Load and play animation files from LittleFS.
- Query and set the active emotion.
- Create/update/delete emotion definitions (including ear color/gradient metadata).

### 2) Ear LED controls
- Read current ear state.
- Set brightness as percent or raw 0-255 value.

### 3) Fan controls
- Read current duty cycle.
- Set duty cycle over HTTP endpoint.

### 4) File management
- Upload animation assets.
- List files and data partition usage.
- Read/delete files.

### 5) System and diagnostics
- Heap usage endpoint.
- Gyro/tilt endpoint.
- Static content hosting from `data/`.
- OTA updates through ElegantOTA.

### 6) BLE remote control
- Query available emotions/capabilities.
- Switch emotion.
- Trigger named capabilities (brightness up/down, fan speed up/down).

## Web/API endpoints

Base URL (default AP): `http://192.168.4.1`

- `GET /heap`
- `GET /gyro`
- `GET /emotions`
- `GET|POST|PUT|DELETE /emotion`
- `PUT /emotion/current`
- `GET|PUT /fan`
- `GET|PUT /ears`
- `GET /capabilities`
- `GET /files`
- `GET /files-info`
- `GET|POST|DELETE /file`

Ready-to-run API samples are included in `test/http-files/*.http`.

## Tools and dependencies

### Build system
- [PlatformIO](https://platformio.org/) using Arduino framework (`espressif32` platform).

### Key libraries
- `ESPAsyncWebServer` + `AsyncTCP`
- `ElegantOTA`
- `NimBLE-Arduino`
- `ArduinoJson`
- `ESP32-HUB75-MatrixPanel-DMA`
- `AnimatedGIF`
- `MPU6050_tockn`
- `Adafruit_NeoPixel`
- `Adafruit GFX` + `Adafruit SSD1306` + `esp32-sh1106-oled`

(Exact versions/sources are in `platformio.ini`.)

## Setup

## 1) Prerequisites
- VS Code + PlatformIO extension **or** PlatformIO Core CLI installed.
- ESP32 board connected over USB.

## 2) Clone and open project
```bash
git clone <your-fork-or-repo-url>
cd protogen-esp32
```

## 3) Configure Wi‑Fi AP credentials
Edit `src/main.cpp`:
- `WIFI_NAME`
- `WIFI_PASS`

You can also adjust pin mappings and panel parameters there.

## 4) Build firmware
```bash
pio run
```

## 5) Upload firmware
```bash
pio run -t upload
```

## 6) Upload static files (LittleFS)
```bash
pio run -t uploadfs
```

## 7) Open serial monitor
```bash
pio device monitor -b 115200
```

## 8) Connect and test
1. Connect to the ESP32 AP (default SSID from `WIFI_NAME`).
2. Open `http://192.168.4.1`.
3. Exercise endpoints using `test/http-files/*.http`.

## OTA updates
Once running, OTA is enabled via ElegantOTA on the async web server. Use the web interface endpoint exposed by ElegantOTA to upload new firmware/images.

## Project layout

- `src/` - firmware modules (controllers, endpoints, models, capabilities)
- `data/` - static web assets and animation files copied to LittleFS
- `test/http-files/` - HTTP request collections for manual endpoint testing
- `platformio.ini` - board/env config and dependencies

## Notes
- This project currently defaults to `ALLOW_ALL_FILE_CHANGES = true` in `src/main.cpp`; review before production use.
- Consider moving credentials out of source code for shared/public deployments.
