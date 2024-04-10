#include <stdio.h>
#include "MPU9250.h"
#include "Math.h"
#include "BluetoothSerial.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;
const int ledPin = 32;


void setup() {
  
  // serial to display data
  pinMode (ledPin, OUTPUT);

  Serial.begin(115200);

  while(!Serial) {}

  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif

  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
    
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.println("Status: ");
    Serial.println(status);
    while(1) {}
  }
  

}

int get_acc() {
  float accX = IMU.getAccelX_mss();
  float accY = IMU.getAccelY_mss();
  float accZ = IMU.getAccelZ_mss();
  Serial.print("accX:");
  Serial.println(accX);
  Serial.print("accY:");
  Serial.println(accY);
  Serial.print("accZ:");
  Serial.println(accZ);
  int acc = (int)abs(9.9-sqrt(accX*accX + accY*accY + accZ*accZ));
  return acc;
}

void loop() {
  IMU.readSensor();
  
  int acc = get_acc();

  Serial.print("Variable_2:");
  Serial.println(acc);

  if(acc>=20){
    digitalWrite (ledPin, HIGH);  // turn on the LED
    SerialBT.write('1');
    SerialBT.write('/n');
    // delay(20000);      
  }
  else{
    SerialBT.write('0');
    SerialBT.write('\n');
  }
  delay(100);
}
