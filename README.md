# VideoLIGHT OS v6.1F

<div align="center">
  <img src="https://via.placeholder.com/800x200.png?text=VideoLIGHT+OS+v6.1F" alt="VideoLIGHT Banner">
</div>

A sophisticated Arduino-based LED controller with gaming capabilities, developed by **[theGrove](https://geniusbar.site/)**.

## 📌 Table of Contents
- [Features](#-features)
- [Hardware Setup](#-hardware-setup)
- [Installation](#-installation)
- [Usage](#-usage)
- [Game Controls](#-game-controls)
- [Technical Specs](#-technical-specs)
- [Links](#-links)

## ✨ Features

### LED Control System
✅ **10 WS2812B NeoPixels**  
✅ **7 Dynamic Effects**:
- Static Color
- Running Dot
- Confetti
- Staboscope
- SOS Signal
- Hazard Lights
- Police Mode

✅ **Full RGB Control**  
✅ **Brightness Adjustment**  
✅ **EEPROM Memory** for settings

### 🕹 Built-in Game
- Space Invaders clone
- Progressive difficulty
- Score & lives system

### 🖥 OLED Interface
- Interactive menu system
- Scrolling about screen
- Screensaver mode

## 🛠 Hardware Setup

### Components
| Component | Specification |
|-----------|---------------|
| Microcontroller | Arduino Uno/Nano |
| Display | SSD1306 128x64 (I2C) |
| LEDs | WS2812B (10x) |
| Buttons | 2x Tactile |

### Wiring
| Pin | Connection |
|-----|------------|
| D7  | LED Data   |
| D6  | SELECT     |
| D5  | NEXT       |

## 💻 Installation

1. Install required libraries:
   ```bash
   arduino-cli lib install "FastLED" "Adafruit SSD1306" "Adafruit GFX Library"
2. Upload the sketch::
   ```bash
   arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno VideoLIGHT6F.ino

🎛 Usage
Menu Navigation
Action  Button
Navigate  NEXT
Select/Back SELECT
Settings
Adjust brightness (0-255)

Set RGB values

Select LED effect

Toggle display inversion

🎮 Game Controls
Control Action
SELECT  Move Left
NEXT  Move Right
Auto  Shooting