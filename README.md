# Wireless-Temperature-Sensor

OVERVIEW:
A wireless temperature sensor employing 8 three wire PT100 RTDs transmitting the acquired data using Bluetooth.

The aim of this project was to measure and wirelessly transmit temperature data from eight three wire PT100 RTD temperature sensors. In order to avoid having multiple ADC/Transducer ICs for conversion of the sensor data, I came across the ADS1248 multiplexer/ADC for PT100 RTDs. The ADS1248 can accomodate 4 two wire RTDs and 2 three wire RTDs. Three wire RTDs cancel out the additional resistance introducted by the leads of the wires and are 
