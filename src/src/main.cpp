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
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Thread.h"
#include <ThreadController.h>
#include <utility/imumaths.h>
#include <SPIMemory.h>
#include "main.h"

// ThreadController that will controll all threads
ThreadController thread_control = ThreadController();

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
double xPos = 0, yPos = 0, headingVel = 0;
uint16_t BNO055_SAMPLERATE_DELAY_MS = 50; //how often to read data from the board
uint16_t PRINT_DELAY_MS = 50; // how often to print the data
uint16_t printCount = 0; //counter to avoid printing every 10MS sample

//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

//Threads (as a pointer)
Thread* ThreadBNO055 = new Thread();
Thread* ThreadMPL3115A2 = new Thread();

// This function calls BNO055 specific functions
// for Orientation Data, Angular Velocity Data, and Linear Acceleration Data
void BNO055()
{
  //
  unsigned long tStart = micros();
  sensors_event_t orientationData , linearAccelData, angVelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.x / cos(DEG_2_RAD * orientationData.orientation.x);

  if (printCount * BNO055_SAMPLERATE_DELAY_MS >= PRINT_DELAY_MS) {
    //enough iterations have passed that we can print the latest data
    Serial.print("Heading: ");
    Serial.println(orientationData.orientation.x);
    Serial.print("Position: ");
    Serial.print(xPos);
    Serial.print(" , ");
    Serial.println(yPos);
    Serial.print("Speed: ");
    Serial.println(headingVel);
    Serial.print("GryoSope: ");
    Serial.println(angVelData.gyro.x);
    Serial.println(angVelData.gyro.y);
    Serial.println(angVelData.gyro.z);
    Serial.println("-------");

    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }
  int8_t boardTemp = bno.getTemp();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000))
  {
    //poll until the next sample is ready
  }
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
  delay(5000);

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
	ThreadBNO055->setInterval(50);

  	// Configure ThreadTwo
	ThreadMPL3115A2->onRun(MPL3115A2);
	ThreadMPL3115A2->setInterval(250);

	// Adds both threads to the controller
	thread_control.add(ThreadBNO055);
  thread_control.add(ThreadMPL3115A2);
}

void loop() {
  // run ThreadController
	// this will check every thread inside ThreadController,
	thread_control.run();
}