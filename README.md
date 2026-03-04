# Wearable Device for Assisting Deaf and Hard-of-Hearing Individuals in Identifying Sound-Based Hazards

## Description

This project aims to develop a **wearable device** designed to assist **deaf and hard-of-hearing individuals** in identifying potential dangers that are typically detected through sound signals.

The device is composed of the following components:

- **Microcontroller**
- **MEMS Microphone**
- **Vibration Motor**
- **OLED Display**

The system captures environmental sounds and notifies the user about potential hazards through **vibration feedback** and **visual alerts on an OLED display**. Sound classification is performed in the **cloud using neural networks**, ensuring higher accuracy in detecting dangerous events.

---

## Features

- **Audio Capture**  
  A MEMS microphone captures environmental sounds.

- **Signal Processing and Transmission**  
  The microcontroller processes the captured signal, builds an audio file, and sends it to a cloud server via WiFi.

- **Sound Classification**  
  The cloud server uses neural networks to classify the audio and identify possible hazards.

- **User Notification**  
  When a hazard is detected, the device alerts the user through **vibration feedback** and displays the type of hazard on the **OLED screen**.

---

## Technologies Used

- **Microcontroller:** ESP32-WROOM  
- **MEMS Microphone:** INMP441  
- **OLED Display:** SSD1306  
- **Machine Learning:** Cloud-based sound classification using neural networks  
- **Communication:** WiFi with HTTP/HTTPS protocols

---

## How It Works

1. The MEMS microphone captures environmental sounds and the microcontroller processes the signal.
2. The audio signal is transmitted to a cloud server via WiFi.
3. The cloud server uses neural networks to classify the audio and detect potential hazards.
4. The microcontroller receives the classification result and notifies the user using vibrations and messages on the OLED display.

---

## Installation

1. **Clone the repository**
2. **Install the required dependencies**
3. **Configure the environment according to the instructions in `config.h`**
4. **Compile and upload the firmware to the microcontroller**

---

## Contact

For more information, please contact:

- **Email:** phlangone@gmail.com  
- **GitHub:** phlangone
