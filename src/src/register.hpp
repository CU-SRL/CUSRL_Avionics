#ifndef _REGISTER_HPP_
#define _REGISTER_HPP_

//-------------------------------------------------------
//                   BNO055 Registers
//-------------------------------------------------------

//-------------------------------------------------------
//                  MPL3115A2 Registers
//-------------------------------------------------------

#define MPL3115_I2C_ADDR             0x60 // I2C address
#define MPL3115_STATUS               0x00 // Sensor Status Register
#define MPL3115_OUT P_MSB            0x01 // Pressure Data Out MSB
#define MPL3115_OUT_P_CSB            0x02 // Pressure Data Out CSB
#define MPL3115_OUT_P_LSB            0x03 // Pressure Data Out LSB
#define MPL3115_OUT_T_MSB            0x04 // Temperature Data Out MSB
#define MPL3115_OUT_T_LSB            0x05 // Temperature Data Out LSB
#define MPL3115_DR_STATUS            0x06 // Sensor Status Register
#define MPL3115_OUT_P_DELTA MSB      0x07 // Pressure Data Out Delta MSB
#define MPL3115_OUT_P_DELTA_CSB      0x08 // Pressure Data Out Delta CSB
#define MPL3115_OUT_P_DELTA_LSB      0x09 // Pressure Data Out Delta LSB
#define MPL3115_OUT_T_DELTA_MSB      0x0A // Temperature Data Out Delta MSB
#define MPL3115_OUT_T_DELTA_LSB      0x0B // Temperature Data Out Delta LSB
#define MPL3115_WHO_AM_I             0x0C // Device Identification Register
#define MPL3115_F_STATUS             0x0D // FIFO Status Register
#define MPL3115_F_DATA               0X0E // FIFO 8-bit Data Access
#define MPL3115_F_SETUP              0x0F // FIFO Setup Register
#define MPL3115_TIME_DLY             0x10 // Time Delay
#define MPL3115_SYSMOD               0x11 // System Mode Register
#define MPL3115_INT_SOURCE           0x12 // Interrupt Source Register
#define MPL3115_PT_DATA_CFG          0x13 // PT Data Configuration Register
#define MPL3115_BAR_IN_MSB           0x14 // BAR Input in MSB
#define MPL3115_BAR_IN_LSB           0x15 // BAR Input in LSB
#define MPL3115_P_TGT_MSB            0x16 // Pressure Target MSB
#define MPL3115_P_TGT_LSB            0x17 // Pressure Target LSB
#define MPL3115_T_TGT                0x18 // Temperature Target
#define MPL3115_P_WND_MSB            0x19 // Pressure/Altitude Window MSB
#define MPL3115_P_WND_LSB            0x1A // Pressure/Altitude Window LSB
#define MPL3115_T_WND                0X1B // Temperature Window
#define MPL3115_P_MIN_MSB            0x1C // Minimum Pressure Data Out MSB
#define MPL3115_P_MIN_CSB            0x1D // Minimum Pressure Data Out CSB
#define MPL3115_P_MIN_LSB            0x1E // Minimum Pressure Data Out LSB
#define MPL3115_T_MIN_MSB            0x1F // Minimum Temperature Data Out MSB
#define MPL3115_T_MIN_LSB            0x20 // Minimum Temperature Data Out LSB
#define MPL3115_P_MAX_MSB            0x21 // Maximum Pressure Data Out MSB
#define MPL3115_P_MAX_CSB            0x22 // Maximum Pressure Data Out CSB
#define MPL3115_P_MAX_LSB            0x23 // Maximum Pressure Data Out LSB
#define MPL3115_T_MAX_MSB            0X24 // Maximum Temperature Data Out MSB
#define MPL3115_T_MAX_LSB            0x25 // Maximum Temperature Data Out LSB
#define MPL3115_CTRL_REG1            0x26 // Control Register 1
#define MPL3115_CTRL_REG2            0x27 // Control Register 2
#define MPL3115_CTRL_REG3            0x28 // Control Register 3
#define MPL3115_CTRL_REG4            0x29 // Control Register 4
#define MPL3115_CTRL_REG5            0x2A // Control Register 5
#define MPL3115_OFF_P                0x2B // Pressure Data User Offset
#define MPL3115_OFF_T                0x2C // Temperature Data User Offset
#define MPL3115_OFF_H                0x2D // Altitude Data User Offset Register

#endif