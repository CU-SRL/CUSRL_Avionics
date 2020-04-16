#include <yonics.hpp>

struct GPSdata  {
    float lat = 0;
    float lon = 0;
    float altitude = 0;
    float speed = 0;
    float angle = 0;
    float sat_num = 0;
    uint32_t t;
};

// CLASS DEFINITION
class DigitalGPS {
    private:
    public:
        Adafruit_GPS* GPS;
        HardwareSerial* GPSSerial;

        DigitalGPS(HardwareSerial *ser);

        void dummyPrint();
        void initGPS();
        void eraseLOCUS();
        void GPSData_dump_setup();
        void refresh_GPSData(bool gpsecho);
        void pullGPSFlashData();
        void pullRawGPS(/*GPSdata* data*/);
};

DigitalGPS::DigitalGPS(HardwareSerial *ser)
{
    DigitalGPS::GPS = new Adafruit_GPS(ser);
    // Initialize MTK3339 GPS Unit
    DigitalGPS::GPS->begin(9600);

    DigitalGPS::GPSSerial=ser;
}

void DigitalGPS::initGPS()
{
    // RMC (recommended minimum) and GGA (fix data) including altitude
    DigitalGPS::GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    DigitalGPS::GPS->sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // 10 Hz update rate

    if (!DigitalGPS::GPS->LOCUS_StartLogger())
    {
        Serial.println("Failed to start GPS LOCUS Logger");
        return;
    }
}

void DigitalGPS::eraseLOCUS()
{
    DigitalGPS::GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
    DigitalGPS::GPS->sendCommand(PMTK_LOCUS_ERASE_FLASH); // Erase LOCUS Flash Chip
}

void DigitalGPS::GPSData_dump_setup()
{
    // TURN OFF OUTPUT
    DigitalGPS::GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);  

    while(DigitalGPS::GPSSerial->available())
    {
      DigitalGPS::GPSSerial->read();
    }

    delay(1000);
    DigitalGPS::GPS->sendCommand("$PMTK622,1*29");   
}

void DigitalGPS::dummyPrint()
{
    Serial.println("Hello World!!");
}

//RUDIMENTARY PULLS TO SERIAL AND THEN YOU HAVE TO COPY TO A TXT FILE TO PARSE FROM PYTHON UNTIL PARSER IS COMPLETE
void DigitalGPS::pullGPSFlashData()
{
  if (DigitalGPS::GPSSerial->available())
  {
    char c = DigitalGPS::GPSSerial->read();
    Serial.write(c);
  }
}

void DigitalGPS::refresh_GPSData(bool GPSECHO)
{
    // read data from the GPS
    char c = DigitalGPS::GPS->read();

    // GPSECHO if set to true will print the raw NMEA strings
    if (GPSECHO)
    {
        if (c) 
        {
            Serial.print(c);
        }
    }
    // Check to See if sentenced recieved...
    if (DigitalGPS::GPS->newNMEAreceived()) 
    {
      Serial.println(DigitalGPS::GPS->lastNMEA()); // this also sets the newNMEAreceived() flag to false
      if (!DigitalGPS::GPS->parse(DigitalGPS::GPS->lastNMEA())) // this also sets the newNMEAreceived() flag to false
        return; // we can fail to parse a sentence in which case we should just wait for another
    }
}
void DigitalGPS::pullRawGPS(/*GPSdata* data*/)
{
    if(DigitalGPS::GPS->fix)
    {
        //data->altitude = DigitalGPS::GPS->altitude;
        /*Serial.print("Altitude: ");
        Serial.print(DigitalGPS::GPS->altitude);

        //data->angle = DigitalGPS::GPS->angle;
        Serial.print(" Angle: ");
        Serial.print(DigitalGPS::GPS->angle);

        //data->lat = DigitalGPS::GPS->latitudeDegrees;
        Serial.print(" Lat (Deg): ");
        Serial.print(DigitalGPS::GPS->latitudeDegrees);

        //data->lon = DigitalGPS::GPS->longitudeDegrees;
        Serial.print(" Long (Deg): ");
        Serial.print(DigitalGPS::GPS->longitudeDegrees);

        //data->sat_num = DigitalGPS::GPS->satellites;
        Serial.print(" Sats: ");
        Serial.print(DigitalGPS::GPS->satellites);

        //data->speed = DigitalGPS::GPS->speed;
        Serial.print(" Speed: ");
        Serial.println(DigitalGPS::GPS->speed);*/
    }
}
