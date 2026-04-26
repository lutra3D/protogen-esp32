# protogen-esp32

Project is a firmware for **ESP32-based Protogens**. It is intended to be modular and extensible, and can be controlled over a **Web UI**, **Bluetooth remote**, or **REST API**.

## What the UI provides
The built-in web interface (served from `data/`) gives a quick control surface for:
- switching the active emotion/animation,
- adjusting ear LED brightness,
- controlling fan speed,
- viewing basic device status and diagnostics,
- managing files/emotion assets without reflashing firmware.

The same functionality is also exposed by HTTP endpoints for automation and external apps.

## Capabilities

### Emotion playback and management
- Load and play animation files from LittleFS.
- Query and set the active emotion.
- Create/update/delete emotion definitions (including ear color/gradient metadata).

### Ear LED controls
- Read current ear state.
- Set brightness as percent or raw 0-255 value.

### Fan controls
- Read current duty cycle.
- Set duty cycle over HTTP endpoint.

### File management
- Upload animation assets.
- List files and data partition usage.
- Read/delete files.

### System and diagnostics
- Heap usage endpoint.
- Gyro/tilt endpoint.
- Static content hosting from `data/`.
- OTA updates through ElegantOTA.

### BLE remote control
- Query available emotions/capabilities.
- Switch emotion.
- Trigger named capabilities (brightness up/down, fan speed up/down).

## Hardware/firmware architecture

The firmware composes several controllers and managers:
- `FaceDisplay` renders animation files to a chained 64x32 HUB75 matrix setup.
- `EarController` drives ear LEDs (NeoPixel-compatible strip/ring).
- `FanController` controls fan speed through PWM.
- `TiltController` reads motion/tilt data over I2C.
- `WebServerManager` serves the API and static web assets over Wi‑Fi AP mode.
- `BLEController` exposes a BLE service/characteristic for remote commands.
- `SettingsStorage` persists runtime-adjustable settings.

See `src/main.cpp` for wiring and startup order.

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

Ready-to-run API samples are in `test/http-files/*.http`.

## Tools and dependencies

- Build: [PlatformIO](https://platformio.org/) (Arduino framework, `espressif32`).
- Core libs: `ESPAsyncWebServer`, `AsyncTCP`, `ElegantOTA`, `NimBLE-Arduino`, `ArduinoJson`, `ESP32-HUB75-MatrixPanel-DMA`, `AnimatedGIF`, `MPU6050_tockn`, `Adafruit_NeoPixel`, `Adafruit GFX`, `Adafruit SSD1306`, `esp32-sh1106-oled`.
- Exact versions/sources: `platformio.ini`.

## Setup (compact)

1. Install PlatformIO (VS Code extension or CLI).
2. Clone and enter the repo:
   ```bash
   git clone <your-fork-or-repo-url>
   cd protogen-esp32
   ```
3. Optional: edit `WIFI_NAME` / `WIFI_PASS` in `src/main.cpp`.
4. Build + flash + filesystem:
   ```bash
   pio run
   pio run -t upload
   pio run -t uploadfs
   ```
5. Monitor serial output:
   ```bash
   pio device monitor -b 115200
   ```
6. Connect to the AP and open `http://192.168.4.1`.

## OTA updates

OTA is enabled via ElegantOTA on the async web server.

## Project layout

- `src/` - firmware modules (controllers, endpoints, models, capabilities)
- `data/` - static web assets and animation files copied to LittleFS
- `test/http-files/` - HTTP request collections for manual endpoint testing
- `platformio.ini` - board/env config and dependencies

## Notes

- Default `ALLOW_ALL_FILE_CHANGES = true` in `src/main.cpp`; review before production use.
- Consider moving credentials out of source code for shared/public deployments.
