# STIJ5014: Distributed Systems (Assignment 3)

### Lecturer: Suwannit Chareen Chit a/l Sop Chit

| Student Name              | Matric Number |
|---------------------------|---------------|
| THARISANA GANESON         | 832071        |

## Title: Temperature, Humidity and Heat Monitoring System
The purpose of this system is to read the current environments Temperature, Humidity, and Heat Level. To read that the system uses a NodeMCU microcontroller to monitor environmental data like temperature, humidity, and heat index. 
This system employs a DHT11 sensor to collect data and when crucial thresholds are exceeded, notification is sent via Telegram. In addition, the value of maximum, maximum and average are stored into EEPROM and then displays the 
latest data through the web interface that is specially built for this.

## List of components
Microcontroller: NodeMCU (ESP8266) <br/>
Sensor: DHT11 for temperature and humidity measurements <br/>
Storage: EEPROM for storing statistical data <br/>
Wi-Fi Connetion: Integrated in the NodeMCU for connecting to the internet <br/>
Output: <br/>
&nbsp; &nbsp; Web interface hosted on the NodeMCU for displaying data <br/>
Notification: <br/>
&nbsp; &nbsp; notification sent to Telegram <br/>

## Installation
## Hardware Requirements
&nbsp; NodeMCU (ESP8266) <br/>
&nbsp; DHT11 Sensor <br/>
&nbsp; Jumper Wires <br/>

## Software Requirements
Arduino IDE <br/>
ESP8266 Board Package installed in Arduino IDE <br/>
Libraries: <br/>
&nbsp; DHT.h <br/> 
&nbsp; EEPROM.h <br/>
&nbsp; ESP8266WiFi.h <br/>
&nbsp; UniversalTelegramBot.h <br/>

## System Flow Explanation
I.	The NodeMCU ESP8266 is a microcontroller with WiFi functionality. It collects sensor data, interprets it, and provides local web hosting and internet connectivity for Telegram messages. <br/>
II.	DHT11 Sensor is responsible to collects environmental data like temperature, humidity, and heat index data.<br/>
III.	EEPROM: Used for maintaining minimum, maximum, and average temperature, humidity, and heat index values from multiple sample data. <br/>
IV.	The Universal Telegram Bot Library enables the NodeMCU to transmit notifications to Telegram chats. <br/>
V.	Web Interface: The NodeMCU displays real-time sensor and calculated data, including minimum, maximum, and average temperatures, humidity, and heat index. <br/>

## Presentation
Presentation video in the following link:( https://youtu.be/SbFtsGjF4tA)
