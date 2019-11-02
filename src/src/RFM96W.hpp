#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// RFM96W Server is the ground station
class RFM96W_Server
{
    public:
        RFM96W_Server(uint8_t slaveSelectPin, uint8_t interruptPin, RHHardwareSPI hardware_spi); // Constructor
        ~RFM96W_Server(); // Destructor

        RH_RF95 *Server_Driver;
        RHReliableDatagram *Server_Manager;

        void init();
        void loop();
        void TX();
        void RX();

    private:
    protected:
};


// RFM96W Client is the rocket station
class RFM96W_Client
{
    public:
        RFM96W_Client(uint8_t slaveSelectPin, uint8_t interruptPin, RHHardwareSPI hardware_spi); // Constructor
        ~RFM96W_Client();

        RH_RF95 *Client_Driver;
        RHReliableDatagram *Client_Manager;

        void init();
        void loop(uint8_t data);
        void TX(const uint8_t *data);
        void RX();

    private:
    protected:
};

#endif