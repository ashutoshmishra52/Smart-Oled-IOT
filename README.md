# Smart IoT Display System (ESP32)

A compact IoT-based display system built using ESP32 that shows real-time temperature, time, and notifications on an OLED screen. It also supports remote control via Telegram Bot.

---

## Features

- Real-time temperature monitoring using DHT11
- Live time and date using NTP
- OLED display with custom UI
- Animated eye interface for visual interaction
- Telegram Bot integration for remote control
- Buzzer alert system for high temperature and commands
- Custom message display from Telegram
- Works on both USB and 3.7V Li-ion battery

---

## Components Used

- ESP32 DevKit V1  
- DHT11 Temperature Sensor  
- 0.96" OLED Display (I2C)  
- Buzzer  
- 3.7V Li-ion Battery  
- TP4056 Charging Module (optional)  
- Breadboard  
- Jumper Wires  

---

## Hardware Connections

### OLED Display (I2C)
- VCC → 3.3V  
- GND → GND  
- SDA → GPIO 21  
- SCL → GPIO 22  

### DHT11 Sensor
- VCC → 3.3V  
- GND → GND  
- DATA → GPIO 4  

### Buzzer
- Positive → GPIO 2  
- Negative → GND  

---

## Power Supply

### Option 1: USB
- Directly power ESP32 using USB cable  

### Option 2: 3.7V Li-ion Battery
- Battery → TP4056 module  
- TP4056 OUT+ → ESP32 VIN (5V)  
- TP4056 OUT- → GND  

---

## How It Works

- ESP32 connects to WiFi and syncs time using NTP  
- DHT11 reads temperature and updates every few seconds  
- OLED displays time, temperature, and animations  
- Telegram Bot allows:
  - Checking temperature  
  - Triggering buzzer  
  - Displaying custom messages  
- If temperature crosses threshold, buzzer activates automatically  

---

## Telegram Commands

- /start → Initialize bot  
- Get Temperature → Shows current temperature  
- Eye → Activates eye animation  
- Buzzer → Triggers buzzer  

---

## Setup Instructions

1. Install Arduino IDE or PlatformIO  
2. Install required libraries:
   - WiFi  
   - HTTPClient  
   - ArduinoJson  
   - Adafruit GFX  
   - Adafruit SSD1306  
   - DHT sensor library  

3. Update credentials in code:
   ```cpp
   const char* ssid = "YOUR_WIFI";
   const char* pass = "YOUR_PASSWORD";
   String botToken = "YOUR_TELEGRAM_BOT_TOKEN";
   Upload code to ESP32
4. Connect hardware as per wiring
5. Power the device




## 🔌 Full Hardware Connections (ESP32 IoT Display)

### 🧠 ESP32 DevKit V1 Pin Mapping

-----------------------------------
🔹 OLED Display (I2C - SSD1306)
-----------------------------------
VCC  →  3.3V  
GND  →  GND  
SDA  →  GPIO 21  
SCL  →  GPIO 22  

-----------------------------------
🌡️ DHT11 Temperature Sensor
-----------------------------------
VCC  →  3.3V  
GND  →  GND  
DATA →  GPIO 4  

(Note: Add 10k pull-up resistor between DATA and VCC if not prebuilt)

-----------------------------------
🔔 Buzzer
-----------------------------------
Positive (+) → GPIO 2  
Negative (-) → GND  

-----------------------------------
🔋 Power Supply Options
-----------------------------------

Option 1: USB Power  
→ Directly power ESP32 via USB cable  

Option 2: 3.7V Li-ion Battery  

Battery (+) → TP4056 B+  
Battery (-) → TP4056 B-  

TP4056 OUT+ → ESP32 5V (VIN)  
TP4056 OUT- → ESP32 GND  

(Recommended: Use a boost converter if voltage drops under load)

-----------------------------------
🧩 Assembly
-----------------------------------
All components are connected using a breadboard and jumper wires for easy prototyping and testing.

-----------------------------------
⚠️ Important Notes
-----------------------------------
• All components must share a COMMON GND  
• Use 3.3V (NOT 5V) for OLED and DHT11  
• ESP32 GPIO pins are 3.3V logic  
• Ensure stable power supply for WiFi usage  
