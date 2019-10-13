#include <yonics.hpp>

DigitalGPS::DigitalGPS(HardwareSerial *ser)
{
    Adafruit_GPS initGPS(ser);
    // Initialize MTK3339 GPS Unit
    initGPS.begin(9600);

    // Pass the object pointers
    GPS = &initGPS;
    GPSSerial=ser;
}

void DigitalGPS::initGPS()
{
    // RMC (recommended minimum) and GGA (fix data) including altitude
    GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS->sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // 10 Hz update rate

    if (!GPS->LOCUS_StartLogger())
    {
        // Serial.println("Failed to start GPS LOCUS Logger");
        return;
    }
}

void DigitalGPS::eraseLOCUS()
{
    GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
    GPS->sendCommand(PMTK_LOCUS_ERASE_FLASH); // Erase LOCUS Flash Chip
}

void DigitalGPS::GPSData_dump_setup()
{
    // TURN OFF OUTPUT
    GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);  

    while(GPSSerial->available())
    {
      GPSSerial->read();
    }

    delay(1000);
    GPS->sendCommand("$PMTK622,1*29");   
}

//RUDIMENTARY PULLS TO SERIAL AND THEN YOU HAVE TO COPY TO A TXT FILE TO PARSE FROM PYTHON UNTIL PARSER IS COMPLETE
void DigitalGPS::pullGPSFlashData()
{
  if (GPSSerial->available())
  {
    char c = GPSSerial->read();
    Serial.write(c);
  }
}

void DigitalGPS::refresh_GPSData(bool GPSECHO)
{
    // read data from the GPS
    char c = GPS->read();

    // GPSECHO if set to true will print the raw NMEA strings
    if (GPSECHO)
    {
        if (c) 
        {
            Serial.print(c);
        }
    }
    // Check to See if sentenced recieved...
    if (GPS->newNMEAreceived()) 
    {
      Serial.println(GPS->lastNMEA()); // this also sets the newNMEAreceived() flag to false
      if (!GPS->parse(GPS->lastNMEA())) // this also sets the newNMEAreceived() flag to false
        return; // we can fail to parse a sentence in which case we should just wait for another
    }
}
void DigitalGPS::pullRawGPS(GPSdata* data)
{
    data->altitude = GPS->altitude;
    data->angle = GPS->angle;
    data->lat = GPS->latitudeDegrees;
    data->lon = GPS->longitudeDegrees;
    data->sat_num = GPS->satellites;
    data->speed = GPS->speed;
}