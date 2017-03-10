# Wireless-Temperature-Sensor

OVERVIEW:
A wireless temperature sensor employing 8 three wire PT100 RTDs transmitting the acquired data using Bluetooth.

The aim of this project was to measure and wirelessly transmit temperature data from eight three wire PT100 RTD temperature sensors. In order to avoid having multiple ADC/Transducer ICs for conversion of the sensor data, I came across the ADS1248 multiplexer/ADC for PT100 RTDs. The ADS1248 can accomodate 4 two wire RTDs and 2 three wire RTDs - using the internal current source of the ADS1248. 

The advantage of three wire RTDs over two wire RTDs is that they cancel out the additional resistance introducted by the leads of the RTDs.

In order to accomodate 8 RTDs for the application, therefore, 4xADS1248 ICs were employed each connected to two RTDs for temperature measurement. The ICs are referred to as CS1, CS2, CS3 and CS4 in the code and access to each channel in each case is referred to as CH1 and CH2.

Each ADS1248 IC was accessed with a 74HC138 3:8 Demultiplexer.

For the wireless transmission of data, the RN42-XV module was used with Bluetooth 2.0. 

The project folder contains:
1. Arduino_Code: The Arduino code for setting up and reading the temperature data from each of the sensors using the ADS1248. The code also includes the setting up and the bluetooth device and for transmitting the data.
2. Schematic_Overview: A schematic of the ICs, microcontrollers and sensors employed.

NOTE: One of my biggest objectives in preparing this project repository is making this code accessible to anyone struggling with integrating the ADS1248 and the Arduino or even just setting up the ADS1248 itself. I had multiple conversations with TI in order to figure this out. I really hope posting this here will help anyone else attempting to do the same.
