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

/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

//Threads (as a pointer)
Thread* ThreadOne = new Thread();
Thread* ThreadTwo = new Thread();
Thread* ThreadBlink = new Thread();

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

  delay(10);
}

void TestAsyncFuncTwo()
{
}

void blink()
{
  	// put your main code here, to run repeatedly:
  	digitalWrite(13, LOW); // RX LED on
  	delay(333);
  	digitalWrite(13, HIGH); // RX LED off
  	delay(333);
}

void setup() {
  // put your setup code here, to run once:
	pinMode(13, OUTPUT);
    Serial.begin(115200);

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  // When Configuring threads you have to give it what function to run.
  // Configure ThreadOne
	ThreadOne->onRun(BNO055);
	ThreadOne->setInterval(10);

  	// Configure ThreadTwo
	ThreadBlink->onRun(blink);
	ThreadBlink->setInterval(250);

	// Adds both threads to the controller
	thread_control.add(ThreadOne);
	//thread_control.add(ThreadTwo);
    thread_control.add(ThreadBlink);
}

void loop() {

  // run ThreadController
	// this will check every thread inside ThreadController,
	// if it should run. If yes, he will run it;
	thread_control.run();

  // Rest of code
	float h = 3.1415;
	h/=2;
}