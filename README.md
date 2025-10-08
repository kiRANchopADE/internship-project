# internship-project
🥫 Smart Food Expiry System
📘 Overview

Smart Food Expiry System is an IoT-based solution designed to reduce food wastage by monitoring temperature, humidity, and expiry dates of stored food items. The system alerts users via Blynk App, LCD Display, and Buzzer when any food becomes unsafe or expired.

⚙️ Tech Stack

Microcontroller: ESP32

Sensor: DHT22 (Temperature & Humidity)

Display: I2C LCD 16x2

IoT Platform: Blynk Cloud

Programming Language: C++ (Arduino IDE)

Version Control: Git & GitHub

💡 Working Principle

User adds a food item and its expiry duration using the Blynk app.

ESP32 reads real-time temperature and humidity via DHT22.

The system compares readings with safe limits for each food.

Alerts are triggered through Buzzer, LCD, and Blynk notifications if unsafe or expired.

🚀 Key Features

Real-time environmental monitoring

Automatic expiry and spoilage detection

Cloud alerts and event logging via Blynk

Multi-item tracking with predefined food database

Compact, low-power IoT design

🧩 Hardware Connections
Component	ESP32 Pin
DHT22 Data	GPIO 15
LCD SDA	GPIO 21
LCD SCL	GPIO 22
Buzzer	GPIO 2
🧠 Future Enhancements

Cloud database integration (Firebase / Node.js)

AI-based spoilage prediction

Voice or chatbot-based assistant

Mobile dashboard for multiple storage areas

👨‍💻 Developed By

Kiran Chopde & Harshank Kohad
Smart IoT Solutions Intern Project
📍2025
