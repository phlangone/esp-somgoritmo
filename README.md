# Wearable Device for Sound Hazard Detection  
### Assistive Technology for Deaf and Hard-of-Hearing Individuals

---

## Overview

<img width="376" height="247" alt="image" src="https://github.com/user-attachments/assets/64a11273-b4ac-4068-bb17-339b7168a4de" />

This project presents a **wearable embedded system** designed to assist **deaf and hard-of-hearing individuals** in detecting **critical sound-based events** in their environment.

The solution combines **embedded systems**, **real-time operating systems (RTOS)**, and **cloud-based machine learning inference** to provide reliable hazard detection and user feedback.

The system focuses especially on **industrial and urban safety scenarios**, where sound awareness is crucial.

---

## Social Context

- ~865,000 Brazilians with hearing impairment in working age  
- Only ~8% are formally employed  
- Industrial environments present additional safety challenges  

This project explores how **AI + Embedded Systems** can bridge this gap through **assistive technology**.

---

## System Architecture

### **Server-Based Inference Architecture**

<img width="945" height="539" alt="image" src="https://github.com/user-attachments/assets/60abf1a3-5bba-402e-82c6-a49b8c68e7f8" />

- Lightweight embedded device  
- Audio processing + transmission  
- Cloud-based neural network inference  
- Fast feedback to the user  

### Embedded Side (Device)

- Captures environmental audio
- Performs signal conditioning
- Sends data to the server via WiFi
- Receives classification results
- Notifies the user

### Cloud Side (Server)

- Receives audio samples
- Applies signal processing pipeline
- Extracts features (MFCC)
- Performs classification using CNN
- Returns hazard label

---

## Hardware Components

<img width="586" height="367" alt="image" src="https://github.com/user-attachments/assets/a6cb2830-625f-4555-9ef1-eea6c973a353" />

- **Microcontroller:** ESP32-WROOM  
- **Microphone:** INMP441 (I2S MEMS)  
- **Display:** SSD1306 OLED (I2C)  
- **Actuator:** Vibration Motor  
- **Connectivity:** WiFi  

---

## Firmware Architecture

<img width="1172" height="746" alt="image" src="https://github.com/user-attachments/assets/54e9dea1-4f13-4a10-987d-30c23e49ccbe" />

The firmware is structured using **FreeRTOS**, enabling modular and concurrent execution:

### Main Tasks

- Audio Capture Task  
- Signal Processing Task  
- Communication Task (HTTP/MQTT)   

---

## Signal Processing Pipeline

<img width="980" height="207" alt="image" src="https://github.com/user-attachments/assets/4b03bf52-cf1a-48a7-827d-f7b475a3f262" />

### 1. FIR Filtering

- Order: 100  
- Window: Hamming  
- Noise reduction  

### 2. Feature Extraction

- **MFCC (Mel Frequency Cepstral Coefficients)**  
- Efficient representation for classification  

---

## Machine Learning

<img width="588" height="263" alt="image" src="https://github.com/user-attachments/assets/1ebe958b-ec2d-474c-a2d0-94d382d05ff0" />

- **Model:** CNN  
- **Dataset:** UrbanSound8K  

### Training Setup

- 80 epochs  
- Categorical Cross-Entropy  
- Adam optimizer  

---

## User Feedback

<img width="1266" height="713" alt="image" src="https://github.com/user-attachments/assets/a2d51005-2d65-446b-9713-4b51405af462" />

- Vibration motor → tactile alert  
- OLED display → hazard identification  

---

## How It Works

1. Audio capture  
2. Transmission to server  
3. Signal processing + MFCC  
4. CNN classification  
5. Response to device  
6. User alert  

---

## Limitations & Trade-offs

- Network dependency  
- Latency  
- Security considerations  
- Trade-off vs TinyML  

---

## Future Improvements

- Edge inference (TinyML)  
- Hybrid architecture  
- Latency optimization  
- Industrial validation  

---

## Contact

- **Author:** Paulo H. Langone Miranda  
- **Email:** phlangone@gmail.com  
- **GitHub:** https://github.com/phlangone  

---

## License

Educational and research purposes
