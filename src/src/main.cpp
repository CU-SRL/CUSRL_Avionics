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

// ThreadController that will controll all threads
ThreadController thread_control = ThreadController();

//Threads (as a pointer)
Thread* ThreadOne = new Thread();
Thread* ThreadTwo = new Thread();
Thread* ThreadBlink = new Thread();

void TestAsyncFuncOne()
{
  	Serial.print("COOL! I'm running TestAsyncFuncOne on: ");
	Serial.println(millis());
}

void TestAsyncFuncTwo()
{
  	Serial.print("And I'm running TestAsyncFuncTwo on: ");
	Serial.println(millis());
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
    Serial.begin(9800);
  // When Configuring threads you have to give it what function to run.
  // Configure ThreadOne
	ThreadOne->onRun(TestAsyncFuncOne);
	ThreadOne->setInterval(250);

	// Configure ThreadTwo
	ThreadTwo->onRun(TestAsyncFuncTwo);
	ThreadTwo->setInterval(250);

  	// Configure ThreadTwo
	ThreadBlink->onRun(blink);
	ThreadBlink->setInterval(250);

	// Adds both threads to the controller
	thread_control.add(ThreadOne);
	thread_control.add(ThreadTwo);
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