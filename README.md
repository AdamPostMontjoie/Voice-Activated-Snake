# Voice Activated Snake

A voice-controlled version of the classic Snake game built on a CrowPanel OLED device. The project combines embedded game logic written in Arduino/C++ with speech recognition handled by a Python script using the Vosk speech-to-text model.

Project documentation site: [Voice Activated Snake Google Site](https://sites.google.com/umass.edu/voiceactivatedsnake/home)

## Overview

Voice Activated Snake replaces traditional button or joystick controls with spoken directional commands. The game runs on the CrowPanel device, while a connected computer listens for voice input, processes speech using Vosk, and sends directional commands to the device.

The project was built as an embedded systems / physical computing project from November to December 2025.

## Features

- Classic Snake gameplay on an OLED display
- Voice-controlled directional movement
- Arduino/C++ game loop running on the CrowPanel device
- Python-based speech recognition using the Vosk model
- Real-time communication between the speech recognition script and embedded game
- Score tracking, collision detection, and display rendering
- Project documentation and progress tracking through a public Google Site

## System Architecture

```text
User Voice Command
        |
        v
Python Speech Recognition Script
        |
        v
Vosk Speech-to-Text Model
        |
        v
Parsed Direction Command
        |
        v
CrowPanel / Arduino Device
        |
        v
Snake Game Logic + OLED Display
