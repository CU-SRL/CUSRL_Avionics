#include "RFM96W.hpp"

// SERVER GLOBAL VARIABLES
uint8_t ServerReturnMessage[] = "RECVD";
uint8_t server_buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t server_len = sizeof(server_buf);
uint8_t server_from;

// CLIENT GLOBAL VARIABLES
//uint8_t ClientData[] = "Hello Server!";
uint8_t client_buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t client_len = sizeof(client_buf);
uint8_t client_from;   

/************************************************************/
/*               RFM96W SERVER CLASS FUNCTIONS              */
/************************************************************/

RFM96W_Server::RFM96W_Server(uint8_t slaveSelectPin, uint8_t interruptPin, RHHardwareSPI hardware_spi)
{
    RFM96W_Server::Server_Driver = new RH_RF95(slaveSelectPin, interruptPin);
    // Singleton instance of the radio driver
    /*RFM96W_Server::Server_Driver = new RH_RF95(slaveSelectPin, interruptPin, hardware_spi);

    // Class to manage server message delivery and receipt, using the server driver declared above
    RFM96W_Server::Server_Manager = new RHReliableDatagram(*RFM96W_Server::Server_Driver, SERVER_ADDRESS);*/

    RFM96W_Server::init(); // Initialize the RF Chip
    //Serial.println("3");
}

RFM96W_Server::~RFM96W_Server()
{

}

void RFM96W_Server::init()
{
  if (!RFM96W_Server::Server_Driver->init())
  {
    Serial.println("init failed");  
  }

  /*if (RFM96W_Server::Server_Manager->init())
  {
    Serial.println("RFM96W Server Initialized...");
  }
  else{
    Serial.println("RFM96W Server failed to initialize.");
  }

  RFM96W_Server::Server_Driver->setTxPower(23, false); // Set Max Power at 23 db because need max range and Use BOOST pin therefore set UFO false*/
}

void RFM96W_Server::loop()
{
  RFM96W_Server::RX();
}

void RFM96W_Server::TX()
{
    // Send a reply back to the client
    /*if (!RFM96W_Server::Server_Manager->sendtoWait(ServerReturnMessage, sizeof(ServerReturnMessage), server_from))
    {
        Serial.println("Server -> sendtoWait failed");
    }*/
}

void RFM96W_Server::RX()
{

  if (RFM96W_Server::Server_Driver->available())
  {
    if (RFM96W_Server::Server_Driver->recv(server_buf, &server_len))
    {
      Serial.print("got request: ");
      Serial.println((char*)server_buf);
      
      RFM96W_Server::Server_Driver->send(ServerReturnMessage, sizeof(ServerReturnMessage));
      RFM96W_Server::Server_Driver->waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
    //Serial.println("Waiting from Client...");
    /*if (RFM96W_Server::Server_Manager->available())
    {
      Serial.println("Waiting from Client...");
      if (RFM96W_Server::Server_Manager->recvfromAck(server_buf, &server_len, &server_from))
      {
        Serial.print("got request from : 0x");
        Serial.print(server_from, HEX);
        Serial.print(": ");
        Serial.println((char*)server_buf);

        RFM96W_Server::TX(); // SEND A MESSAGE BACK
      }
    }*/
}

/************************************************************/
/*               RFM96W CLIENT CLASS FUNCTIONS              */
/************************************************************/

RFM96W_Client::RFM96W_Client(uint8_t slaveSelectPin, uint8_t interruptPin, RHHardwareSPI hardware_spi)
{
    // Singleton instance of the radio driver
    RFM96W_Client::Client_Driver = new RH_RF95(slaveSelectPin, interruptPin);

    /*// Class to manage client message delivery and receipt, using the client driver declared above
    RFM96W_Client::Client_Manager = new RHReliableDatagram(*RFM96W_Client::Client_Driver, CLIENT_ADDRESS);*/

    RFM96W_Client::init(); // Initialize the RF_Chip
}

RFM96W_Client::~RFM96W_Client()
{

}

void RFM96W_Client::init()
{
  if (!RFM96W_Client::Client_Driver->init())
  {
    Serial.println("init failed");  
  }
  /*if (!RFM96W_Client::Client_Manager->init())
  {
    Serial.println("RFM96W Client failed to initialize.");
  }

  RFM96W_Client::Client_Driver->setTxPower(23, false); // Set Max Power at 23 db because need max range and Use BOOST pin therefore set UFO false*/
}

void RFM96W_Client::loop(uint8_t data)
{
  Serial.print(data);
  RFM96W_Client::TX((const uint8_t*)data);
}

void RFM96W_Client::TX(const uint8_t *ClientData)
{
  Serial.println("Sending to rf95_server");

  RFM96W_Client::Client_Driver->send(ClientData, sizeof(ClientData));
  
  RFM96W_Client::Client_Driver->waitPacketSent();

  if (RFM96W_Client::Client_Driver->waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (RFM96W_Client::Client_Driver->recv(client_buf, &client_len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)client_buf); 
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(400);

  /*// Send a message to Server
  if (RFM96W_Client::Client_Manager->sendtoWait(ClientData, sizeof(ClientData), SERVER_ADDRESS))
  {
    Serial.println("Client -> Message Sent");

    RFM96W_Client::RX(); // WAIT FOR MESSAGE FROM GROUND STATION
  }
  else
  {
    Serial.println("Client -> sendtoWait failed");
  }
  delay(500);*/
}

void RFM96W_Client::RX()
{
    /*if (RFM96W_Client::Client_Manager->recvfromAckTimeout(client_buf, &client_len, 2000, &client_from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(client_from, HEX);
      Serial.print(": ");
      Serial.println((char*)client_buf);
    }else
    {
      Serial.println("No reply, is RFM96W Ground Station running?");
    }*/
}
