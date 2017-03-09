#include <SPI.h>
#include <SoftwareSerial.h>

// Bluetooth pins
SoftwareSerial chat(8, 9); // RX, TX

// Demultiplexer pins
const int CSEN = 10; // Chip Select
const int S0 = A2;
const int S1 = A3;
const int S2 = A4;

// ADS1248 pins
// Common pins
const int START = A0;
const int RESET = A1;
// Individual pins
const int DRDY1 = 2; // DRDY1 for CS1
const int DRDY2 = 3; // DRDY2 for CS2

// Variables for the reading, conversion and printing of temperature data
unsigned long Data_Read=0x0; // Variable to store 24 bit ADC data
double Resistance_Data=0; // Variable to store converted resistance data
double Temp_Data=0; // Variable to store temperature data

// Constants for the conversion from resistance to temperature
const double A2DConstA = 3.9083E-3; // constant for conversion from resistance to temperature
const double A2DConstB = -5.775E-7; // constant for conversion from resistance to temeperature
int text;

void setup() 
{
  
// intial delay for start up
delay(500);

//Enable Serial
Serial.begin(115200);
chat.begin(115200);
delay(2000); // delay 1s to stabilize serial ports


//----------------- Bluetooth Setup--------------------------------//
chat.print("$$$"); //Enter Command Mode
delay(1000);
chat.print("SF,1\n"); //Master
delay(1000);
chat.print("R,1\n");
delay(1000);
chat.print("$$$"); //Enter Command Mode
delay(1000);
chat.print("SM,1\n"); //Master
delay(1000);
chat.print("SA,0\n"); //Set authentication off
delay(1000);
chat.print("R,1\n");
delay(1000);
chat.print("$$$"); //Enter Command Mode
delay(1000);
chat.print("C,0006667CA0F8\n");
delay(1000);


//------------------------ADS1248 Setup------------------------------//
//Begin setup
Serial.println("Beginning ADS1248 Setup");
delay(1000);

//Enabling the arduino pins for input and output
Serial.println("Setting CSEN,START,RESET and DRDY pins");  
pinMode(CSEN, OUTPUT);
pinMode(START, OUTPUT); // START pin common for all ADS1248
pinMode(RESET, OUTPUT); // RESET pin common for all ADS1248
pinMode(DRDY1, INPUT);
pinMode(DRDY2, INPUT);
pinMode(S0,OUTPUT);
pinMode(S1,OUTPUT);
pinMode(S2,OUTPUT);

  
//SPI Setup for clock and Data Mode
Serial.println("Setup for SPI Clock and Data Mode");
SPI.setClockDivider(SPI_CLOCK_DIV8); //1MHz Bus Speed
SPI.setDataMode(0x04); //SPI_MODE1

delay(2000); //Give you time to open up the Serial monitor as it will restart the Arduino but not ADS1247

// Beginning SPI Communication
Serial.println("Beginning Serial Communication");
SPI.begin(); //Turn on the SPI Bus

//Setting START HIGH
Serial.println("Setting START High");
digitalWrite(START, HIGH);
delay(500);

//Setting RESET HIGH
Serial.println("Setting RESET High");
digitalWrite(RESET, HIGH);

// Enabling Demulitplexer to set CS LOW for the ADS1248 
Serial.println("Setting CSEN HIGH to enable Demultiplexer");
digitalWrite(CSEN, HIGH);
delay(500);

for (int i = 0; i<2; i++)
{
  switch(i)
  {
    case 0: Serial.println("Setting CS1 LOW to enable CS1");
            digitalWrite(S0,LOW);
            digitalWrite(S1,LOW);
            digitalWrite(S2,LOW);
            break;
    case 1: Serial.println("Setting CS2 LOW to enable CS2");
            digitalWrite(S0,HIGH);
            digitalWrite(S1,LOW);
            digitalWrite(S2,LOW);
            break;
//    case 2:
//    case 3:
//    case 4:
//    case 5:
    default: break;
  }
  ADS1248_Setup();
}

//Setting CS HIGH to reset serial interface
Serial .println("\n\n\nSetting CS HIGH to reset Serial Interface");
digitalWrite(CSEN,LOW); 

chat.print("CS1:CH1\t\tCS1:CH2\t\tCS2:CH1\t\tCS2:CH2\n");

}


void loop() 
{ 



//--------------------------------CS1 READ---------------------------------------------//
digitalWrite(CSEN,HIGH); //Issue SYNC
Serial.println("Setting CS1 LOW to enable CS1");
digitalWrite(S0,LOW);
digitalWrite(S1,LOW);
digitalWrite(S2,LOW);
delay(500);

//-----------------------------------CS1: CH1 READ----------------------------------------//

Serial.println("-------------------------------CS1: CHANNEL 1:----------------------------");
CH1_Setup();
while(digitalRead(DRDY1)==HIGH)
{;} 
Temp_Read();
chat.print("\t\t");      

//-----------------------------------CS1: CH2 READ----------------------------------------//
Serial.println("-------------------------------CS1: CHANNEL 2:----------------------------");
CH2_Setup();
while(digitalRead(DRDY1)==HIGH)
{;} 
Temp_Read();
chat.print("\t\t");

//--------------------------------------------------------------------------------------------------------------------------------//

//--------------------------------CS2 READ---------------------------------------------//
digitalWrite(CSEN,HIGH); //Issue SYNC
Serial.println("Setting CS1 LOW to enable CS1");
digitalWrite(S0,HIGH);
digitalWrite(S1,LOW);
digitalWrite(S2,LOW);
delay(500);

//-----------------------------------CS2: CH1 READ----------------------------------------//

Serial.println("-------------------------------CS2: CHANNEL 1:----------------------------");
CH1_Setup();
while(digitalRead(DRDY2)==HIGH)
{;} 
Temp_Read();
chat.print("\t\t");

//-----------------------------------CS2: CH2 READ----------------------------------------//
Serial.println("-------------------------------CS2: CHANNEL 2:----------------------------");
CH2_Setup();
while(digitalRead(DRDY2)==HIGH)
{;} 
Temp_Read();
chat.print("\n");

//--------------------------------------------------------------------------------------------------------------------------------//





      
//digitalWrite(CSEN,LOW);
delay(10);
}

void ADS1248_Setup()
{
        // Resetting the ADS1248 to make sure changes are effective
        Serial.println("Reset to ensure changes are effective");
        SPI.transfer(0x06); //Reset the ADS1248
        delay(2); //Minimum 0.6ms required for Reset to finish.
        
        // Issuing SDATAC to stop continuous conversions
        Serial.println("SDATAC to stop conversions");
        SPI.transfer(0x16); //Issue SDATAC
        
        Serial.println("\n\nENTERING REGISTER SETUP MODE\n\n");
        //----------------------------------------------------------
        // REGISTER (02)H - MUX1 REGISTER
        Serial.print("\nRegister 02: ");
        // Setting the MUX1 Register (02)h to (20)h to setup the internal reference and to set up REFP0 and REFN0 as inputs
        SPI.transfer(0x42); //Set MUX1 Register (02h) Write (20)h
        SPI.transfer(0x00); //Number of bytes to be written - 1
        SPI.transfer(0x20);
        
        delay(50);
        
        //Reading Register (02)h
        SPI.transfer(0x22); //Read Register 0x2
        SPI.transfer(0x00); //N - 1 Bytes To Be Read
        Serial.print(SPI.transfer(0xFF),HEX); 
        //----------------------------------------------------------
        // REGISTER (03)H - SYS0 REGISTER
        Serial.print("\nRegister 03: ");
        // Setting the SYS0 Register (03)h to (22)h
        SPI.transfer(0x43); //Set SYS0 Register (03h) Write (22)h
        SPI.transfer(0x00); //Number of bytes to be written - 1
        SPI.transfer(0x22);
        
        delay(50);
        
        //Reading Register (03)h
        SPI.transfer(0x23); //Read Register 0x3
        SPI.transfer(0x00); //N - 1 Bytes To Be Read
        Serial.print(SPI.transfer(0xFF),HEX); 
        //----------------------------------------------------------
        // REGISTER (0A)H - IDAC0 REGISTER
        Serial.print("\nRegister 0A: ");
        // Setting the IDAC0 Register (0A)h to (06)h
        SPI.transfer(0x4A); //Set IDAC0 Register (0A)h Write (06)h
        SPI.transfer(0x00); //Number of bytes to be written - 1
        SPI.transfer(0x06);
        
        delay(50);
        
        //Reading Register (2A)h
        SPI.transfer(0x2A); //Read Register 0x3
        SPI.transfer(0x00); //N - 1 Bytes To Be Read
        Serial.print(SPI.transfer(0xFF),HEX); 
        //----------------------------------------------------------
        
        //Sending the SYNC command to initiate ADC conversions
        Serial.println("\nSYNC to begin conversions");
        SPI.transfer(0x04); //Issue SYNC
        SPI.transfer(0x04);
        delay(500);
  }

  void Temp_Read()
  {
  
      //SLEEP command
      SPI.transfer(0x02);
      SPI.begin(); 

      // Initialize Data_Read to 0
      Data_Read = 0x0;

      // RDATA command
      SPI.transfer(0x12);
      
      //Read 24 bits of Data
      Data_Read |= SPI.transfer(0xFF); //Write NOP, Read First Byte and Mask to Data_Read
      Data_Read <<= 8; //Left Bit-Shift Data_Read by 8 bits
      Data_Read |= SPI.transfer(0xFF); //Write NOP, Read Second Byte and Mask to Data_Read
      Data_Read <<= 8;
      Data_Read |= SPI.transfer(0xFF); //Write NOP, Read Third Byte and Mask to Data_Read 
      delay(10);

      //Disable communication after reading data
      digitalWrite(CSEN,LOW);  

      //Print data
      Serial.print("24 Bit Data in Binary:");
      Serial.println(Data_Read,BIN);

      //Calculate corresponding resistance
      Resistance_Data = (double)Data_Read*(double)820.0*(double)2.18/(double)16777216.0;

      //Print resistance data
      Serial.print("Converted Resistance Value:");
      Serial.println(Resistance_Data,20);

      //Calculate temperature from resistance
      Temp_Data = -1 * ( A2DConstA / ( 2 * A2DConstB ) - sqrt( 25 * A2DConstA * A2DConstA + A2DConstB * ( Resistance_Data ) - 100 * A2DConstB ) / ( 10 * A2DConstB ) );

      //Print the temperature data
      Serial.print("Temperature:");
      Serial.println(Temp_Data,20);
      chat.print(Temp_Data,2);
      
  }

void CH1_Setup()
{
  SPI.begin();

  // Setting the MUX0 Register (00)h to (0A)h to enable AIN1 and AIN2 as analog inputs
  SPI.transfer(0x40); //Set MUX0 Register (00h) Write (0A)h
  SPI.transfer(0x00); //Number of bytes to be written - 1
  SPI.transfer(0x0A);
  //Reading Register (00)h
  Serial.print("\nRegister 00: ");
  SPI.transfer(0x20); //Read Register 0x0
  SPI.transfer(0x00); //N - 1 Bytes To Be Read
  Serial.print(SPI.transfer(0xFF),HEX); 

  delay(100);
  SPI.transfer(0x62);//SELFOCAL Command
  delay(500);
  
  //  Set current source   
  SPI.transfer(0x4B); //Set IDAC1 Register (0B)h Write (03)h
  SPI.transfer(0x00); //Number of bytes to be written - 1
  SPI.transfer(0x03);
  delay(50);
  //Reading Register (2B)h
  Serial.print("\nRegister 0B: ");
  SPI.transfer(0x2B); //Read Register 0x3
  SPI.transfer(0x00); //N - 1 Bytes To Be Read
  Serial.print(SPI.transfer(0xFF),HEX); 
  
  //Sending the SYNC command to initiate ADC conversions
  Serial.println("\nSYNC to begin conversions");
  SPI.transfer(0x04); //Issue SYNC
  SPI.transfer(0x04);
  delay(500);
  
  //Setting CS HIGH to reset serial interface
  Serial .println("\n\n\nSetting CS HIGH to reset Serial Interface");
  digitalWrite(CSEN,LOW); 
  
  delay(500);
  
  // Begin SPI communication with the device
  digitalWrite(CSEN,HIGH);
  delay(100);
}

void CH2_Setup()
{
  SPI.begin();

  // Begin SPI communication with the device
  digitalWrite(CSEN,HIGH);
  delay(100);
  
  // Setting the MUX0 Register (00)h to (0A)h to enable AIN1 and AIN2 as analog inputs
  SPI.transfer(0x40); //Set MUX0 Register (00h) Write (0A)h
  SPI.transfer(0x00); //Number of bytes to be written - 1
  SPI.transfer(0x2E);
  //Reading Register (00)h
  Serial.print("\nRegister 00: ");
  SPI.transfer(0x20); //Read Register 0x0
  SPI.transfer(0x00); //N - 1 Bytes To Be Read
  Serial.print(SPI.transfer(0xFF),HEX); 

  delay(100);
  SPI.transfer(0x62);//SELFOCAL Command
  delay(500);
  
  //  Set current source   
  SPI.transfer(0x4B); //Set IDAC1 Register (0B)h Write (03)h
  SPI.transfer(0x00); //Number of bytes to be written - 1
  SPI.transfer(0x47);
  //Reading Register (2B)h
  Serial.print("\nRegister 0B: ");
  SPI.transfer(0x2B); //Read Register 0x3
  SPI.transfer(0x00); //N - 1 Bytes To Be Read
  Serial.print(SPI.transfer(0xFF),HEX); 
  
  //Sending the SYNC command to initiate ADC conversions
  Serial.println("\nSYNC to begin conversions");
  SPI.transfer(0x04); //Issue SYNC
  SPI.transfer(0x04);
  delay(500);
  
  //Setting CS HIGH to reset serial interface
  Serial .println("\n\n\nSetting CS HIGH to reset Serial Interface");
  digitalWrite(CSEN,LOW); 
  
  delay(500);
  
  // Begin SPI communication with the device
  digitalWrite(CSEN,HIGH);
  delay(100);
}

