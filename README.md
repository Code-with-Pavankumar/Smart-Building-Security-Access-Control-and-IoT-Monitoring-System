# Smart Building Security, Access Control and IoT Monitoring System

> Embedded security system built on LPC2129 ARM7 microcontroller with multi-level authentication, automatic door control, event logging, and IoT cloud monitoring.

---

## Overview

This project implements a complete smart building security system for controlling and monitoring entry to a building. Only authorized users can enter through RFID card or keypad password authentication. Every access attempt is logged with timestamp and uploaded to the ThingSpeak IoT cloud.

The entire system runs on the **LPC2129 ARM7 microcontroller** programmed in bare-metal Embedded C using Keil µVision4.

---

## Block Diagram

```
                        LPC2129 ARM7
                       ┌─────────────────────────────┐
  RC522 RFID ──SPI───→ │                             │ ──GPIO──→ Green LED
  Matrix Keypad ─GPIO─→│   Authentication Logic      │ ──GPIO──→ Red LED  
  PIR Sensor ───GPIO──→│   Door Control              │ ──GPIO──→ Buzzer
                       │   Event Logging             │ ──PWM───→ Servo Motor
  DS1307 RTC ───I2C───→│   IoT Upload               │ ──GPIO──→ Relay
  AT24C08 EEPROM─I2C──→│                             │
                       │                             │ ──UART0─→ PC Terminal
  ESP-01 WiFi ──UART1─→│                             │ ──UART1─→ ESP-01
  MCP2551 CAN ──CAN───→│                             │ ──CAN───→ CAN Network
                       └─────────────────────────────┘
                                    │
                              16x2 LCD Display
```

---

## Features

- **RFID Authentication** — RC522 reads card UID via SPI, compares against stored UID in EEPROM
- **Keypad Password** — 4×4 matrix keypad for secondary password authentication
- **Two attempts** for both RFID and password before lockout
- **Automatic door control** — Servo motor opens and closes door after successful authentication
- **PIR motion detection** — detects human presence before starting authentication
- **Event logging** — every access attempt saved to AT24C08 EEPROM with RTC timestamp
- **IoT monitoring** — authentication results uploaded to ThingSpeak via ESP-01 WiFi
- **CAN communication** — security events broadcast to other nodes via MCP2551
- **LED + Buzzer indicators** — Green LED for success, Red LED for failure, buzzer for alerts
- **LCD display** — real-time status messages throughout authentication flow
- **UART debug log** — all events printed to PC terminal at 9600 baud

---

## Hardware Components

| Component | Purpose | Interface |
|-----------|---------|-----------|
| LPC2129 ARM7 | Main microcontroller | — |
| RC522 RFID Reader | Card UID reading | SPI |
| 4×4 Matrix Keypad | Password entry | GPIO |
| PIR Sensor (HC-SR501) | Motion detection | GPIO (P0.15) |
| DS1307 RTC | Timestamp for events | I2C |
| AT24C08 EEPROM | Store UID, passwords, logs | I2C |
| ESP-01 WiFi Module | ThingSpeak IoT upload | UART1 |
| MCP2551 CAN Transceiver | Multi-node communication | CAN |
| Servo Motor | Automatic door open/close | PWM (P0.8) |
| Relay Module | Electromagnetic door lock | GPIO |
| 16×2 LCD Display | User interface messages | GPIO (4-bit) |
| Green / Red LED | Access granted / denied | GPIO |
| Buzzer | Alert on authentication failure | GPIO |

---

## Pin Configuration

### Port 0

| Pin | Function | Peripheral |
|-----|---------|-----------|
| P0.0 | TXD0 | UART0 (Debug PC) |
| P0.1 | RXD0 | UART0 (Debug PC) |
| P0.2 | SCL | I2C (RTC + EEPROM) |
| P0.3 | SDA | I2C (RTC + EEPROM) |
| P0.4 | COL1 | Keypad Column 1 |
| P0.5 | COL2 | Keypad Column 2 |
| P0.6 | COL3 | Keypad Column 3 |
| P0.7 | COL4 | Keypad Column 4 |
| P0.8 | PWM4 / TXD1 | Servo Motor / UART1 TX |
| P0.9 | ROW1 / RXD1 | Keypad Row 1 / UART1 RX |
| P0.10 | ROW2 | Keypad Row 2 |
| P0.11 | ROW3 | Keypad Row 3 |
| P0.12 | ROW4 | Keypad Row 4 |
| P0.13 | GPIO | Green LED |
| P0.14 | GPIO | Red LED |
| P0.15 | GPIO | PIR Sensor |
| P0.16 | EINT0 | External Interrupt (Print logs) |
| P0.25 | RD1 | CAN1 RX |

### Port 1

| Pin | Function | Peripheral |
|-----|---------|-----------|
| P1.16 | GPIO | LCD RS |
| P1.17 | GPIO | LCD EN |
| P1.18–P1.21 | GPIO | LCD D4–D7 |

---

## Software Architecture

```
main.c                  — system entry, authentication flow, main loop
├── rfid_func.c         — RFID UID read via UART, compare against EEPROM
├── keypad_driver.c     — 4x4 matrix keypad scan, debounce, password check
├── servo.c             — PWM4 servo motor init, 0° and 90° control
├── uart0_driver.c      — UART0 TX/RX, string, integer, float functions
├── wifi_driver.c       — ESP-01 AT commands, ThingSpeak HTTP upload
├── can1_driver.c       — CAN1 init, TX, RX, EINT0 interrupt handler
├── data_log.c          — RTC read, EEPROM write/read, success/fail logs
├── lcd_driver.c        — 16x2 LCD 4-bit mode, string, integer display
├── i2c_driver.c        — I2C master init, write, read for RTC and EEPROM
└── header.h            — typedefs, all function declarations, struct definitions
```

---

## Authentication Flow

```
PIR detects motion
        ↓
LCD: "Human Detected"
        ↓
RFID scan (2 attempts)
        ↓
    ┌───┴───┐
  Pass    Fail
    │       ↓
    │   Keypad password (2 attempts)
    │       ↓
    │   ┌───┴───┐
    │ Pass    Fail
    ↓    ↓       ↓
    └────┘    Access Denied
         ↓         ↓
    Access       Red LED
    Granted      Buzzer
         ↓       Log to EEPROM
    Green LED    Upload to ThingSpeak
    Servo open   CAN alert
    Log event
    Upload ThingSpeak
    CAN broadcast
```

---

## EEPROM Memory Map (AT24C08)

| Address | Content |
|---------|---------|
| 0 – 11 | RFID UID (12 ASCII characters) |
| 50 – 53 | Password 1 (4 digits) |
| 54 – 57 | Password 2 (4 digits) |
| 256 – 511 | Success authentication logs (7 bytes per record) |
| 512 – 767 | Failed authentication logs (7 bytes per record) |

Each log record = 7 bytes: seconds, minutes, hours, day, date, month, year (BCD from DS1307 RTC)

---

## IoT Integration

**Platform:** ThingSpeak  
**Module:** ESP-01 (AT firmware)  
**Protocol:** HTTP GET over TCP

| ThingSpeak Field | Value | Meaning |
|-----------------|-------|---------|
| field1 | 1 | Access Granted |
| field1 | 0 | Access Denied |
| field2 | 1 | Door Open |
| field2 | 0 | Door Closed |

---

## Build Instructions

**Toolchain:** Keil µVision4  
**Target device:** LPC2129  
**Crystal:** 12MHz (PCLK = 15MHz after PLL)  
**Flash tool:** Flash Magic

1. Clone this repository
2. Open `DE.uvproj` in Keil µVision4
3. Build all (F7)
4. Flash the generated `.hex` file using Flash Magic at 9600 baud
5. Open a serial terminal (9600 baud, 8N1) on UART0 to view debug output

---

## UART Debug Output (Expected)

```
           SMART BUILDING SECURITY

Writing data on EEPROM
PIR MOTION INITIALIZED
RFID UID : 4900C9C08FCF
Password : 5555

Monitoring for Motion...
Human Detected
RFID Initialized
Waiting for RFID Card
RFID Card detected
RFID CARD ID: 4900C9C08FCF
RFID CARD Verified Successfully
Enter Password
Enter a password : ****
Password verified successfully
Password Authentication Success
Door Opened
Door Closed
Uploading to ThingSpeak
Upload Done
```

---

## Project Structure

```
Smart_Building_Security/
├── main.c
├── header.h
├── rfid_func.c
├── keypad_driver.c
├── servo.c
├── uart0_driver.c
├── wifi_driver.c
├── can1_driver.c
├── data_log.c
├── lcd_driver.c
├── i2c_driver.c
└── DE.uvproj
```

---

## Concepts Demonstrated

- Bare-metal embedded C on ARM7 (LPC2129)
- SPI protocol (RC522 RFID reader)
- I2C protocol (DS1307 RTC + AT24C08 EEPROM)
- UART communication (debug terminal + ESP-01 WiFi)
- CAN bus communication (MCP2551 transceiver)
- PWM signal generation (servo motor door control)
- GPIO interrupt (EINT0 for log printing trigger)
- Multi-level authentication state machine
- IoT cloud integration via ThingSpeak
- Non-volatile event logging with RTC timestamp
- Multi-file project structure with Makefile

---

## Developer

**Pavan Kumar**  
B.E. Electronics and Communication Engineering — 2026  
Embedded Systems Training — Vector India, Bangalore  
GitHub: 

---

## License

This project is open source and available under the MIT License.
