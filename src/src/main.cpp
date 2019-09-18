/***************************************************************/
/*                                                             */
/*                                                             */
/*                    CU-SRL AVIONICS TEAM                     */
/*                    MAIN FLIGHT COMPUTER                     */
/*                                                             */
/*                       Jason Popich                          */
/*                       Lyon Foster                           */
/*                       Carter Mak                            */
/*                       Giselle Koo                           */
/*                                                             */
/***************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Thread.h"
#include <ThreadController.h>
#include <utility/imumaths.h>

// ThreadController that will controll all threads
ThreadController thread_control = ThreadController();

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

//Threads (as a pointer)
Thread* ThreadBNO055 = new Thread();
Thread* ThreadMPL3115A2 = new Thread();


// This function is used in conjunction with BNO055
// This function prints the data to serial.
void printEvent(sensors_event_t* event) {
  Serial.println();
  Serial.print(event->type);
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if ((event->type == SENSOR_TYPE_GYROSCOPE) || (event->type == SENSOR_TYPE_ROTATION_VECTOR)) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }

  Serial.print(": x= ");
  Serial.print(x);
  Serial.print(" | y= ");
  Serial.print(y);
  Serial.print(" | z= ");
  Serial.println(z);
}

// This function calls BNO055 specific functions
// for Orientation Data, Angular Velocity Data, and Linear Acceleration Data
void BNO055()
{
	//could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData , angVelocityData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  printEvent(&orientationData);
  printEvent(&angVelocityData);
  printEvent(&linearAccelData);

  int8_t boardTemp = bno.getTemp();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);
}

// This function calls MPL3115A2 specific functions
// for Altimeter, Temperature, and Pressure.
void MPL3115A2()
{
  float pascals = baro.getPressure();
  Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

  float altm = baro.getAltitude();
  Serial.print(altm); Serial.println(" meters");

  float tempC = baro.getTemperature();
  Serial.print(tempC); Serial.println("*C");
}

void setup() {
  // put your setup code here, to run once:
	pinMode(13, OUTPUT);
  Serial.begin(115200);

  /* Initialise the sensor BNO055 */
  if (!bno.begin())
  {
    Serial.print("Couldn't find sensor - BNO055");
    return;
  }

  /* Initialise the sensor MPL3115A2 */
  if (!baro.begin()) 
  {
    Serial.println("Couldnt find sensor - MPL3115A2");
    return;
  }

  // Configure ThreadBNO055
	ThreadBNO055->onRun(BNO055);
	ThreadBNO055->setInterval(100);

  	// Configure ThreadTwo
	ThreadMPL3115A2->onRun(MPL3115A2);
	ThreadMPL3115A2->setInterval(15);

	// Adds both threads to the controller
	thread_control.add(ThreadBNO055);
  thread_control.add(ThreadMPL3115A2);
}

void loop() {
  // run ThreadController
	// this will check every thread inside ThreadController,
	thread_control.run();
}