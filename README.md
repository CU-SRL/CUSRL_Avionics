![SRL Logo](https://www.colorado.edu/studentgroups/cobra/sites/default/files/styles/slider/public/slider/logo_crop.png?itok=jh4F9ZcZ)

# CUSRL_Avionics

This is the repository for the Avionics team of the [Sounding Rocket Laboratory](https://www.colorado.edu/studentgroups/cobra/ "CU Sounding Rocket Laboratory Website") at the University of Colorado, Boulder. 


## Team Members
* [Lyon Foster](https://github.com/LyonFoster "LyonFoster")
* [Jason Popich](https://github.com/japopich "japopich")
* [Carter Mak](https://github.com/cartermak "cartermak")
* [Giselle Koo](https://github.com/gisellegk "gisellegk")
* [Brendan Haines]()
---

## Code Version Notes

<<<<<<< HEAD
### V5 (V3 and V4 minor changes from V2)

#### Components
* Microcontroller: Teensy 4.0
* Flash Storage Chip: MT25QL256ABA8ESF-0SIT TR
* High G Accelerometer: ADXL377
* Accelerometer: ADXL357
* Magnetometer: AK8963N
* Gyroscope: IAM-20380
* CAN Transceiver: SN65HVD230DR
* GPS: MTK3339
* RF: W.I.P.
 

### V2

#### Components

* Main computer: Teensy 3.6
* Flash storage chip: MT25QL256ABA8ESF-0SIT TR
* IMU: Adafruit BNO055 9-DOF Breakout Board
* Barometric Altimeter: Adafruit MPL3115A2
* Accelerometer: Adafruit ADXL377
* GPS: MTK3339

### V1
=======
### SPI Flash Storage Notes
>>>>>>> 51ff3efac751bb77e930d8d1d45736bd63eb3926

For computer V2, I tried getting fancy with the flash library, trying to make it flexible enough to add saving of a variety of different data types without having to hardcode each type.

Since then, I've concluded that, without a proper file system on the flash chip, the easiest way to create a *robust* set of routines is preciesly to hardcode support for each data structure we store. In particular, it is important to be able to predictably *read* the data stored on the flash chip even after all volatile memory has been closed, i.e. the flight computer has been reset. 

<<<<<<< HEAD
### Flights
=======
Thus, the current revision will use the following architechture for the non-volatile flash storage of data. 
>>>>>>> 51ff3efac751bb77e930d8d1d45736bd63eb3926

#### Step 1: Initialization

For each data structure stored on the chip, we save the following values:

- `uint32_t addr_start_XXX`: Starting memory address for the block of storage allocated to data type XXX. 
- `uint8_t size_XXX`: The size of a single sample, in bytes, a.k.a. the size of one instance of the data struct.
- `uint32_t nSamples_XXX`: The number of samples of the given type which are currently stored on the flash chip.
- `float freq_XXX`: The samping frequency, in Hz, of the given data type. 

When the system is powered on, all data stored on the flash chip are read and stored to the SD card (via corresponding hardcoded functions in the SD class). The `nSamples` and `addr_start` parameters for each data type are populated based on information stored in the first bytes of the flash chip. (A description of the memory addressing is below).

The `size` and `freq` values are defined by the function `setXXX(uint8_t size, float frequency)`. This function is called when initializing the data types.

#### Step 2: Reading out data

Based on each data type's start address and number of samples, the reading functions can read back the data stored on the flash chip.

The function `readXXX(datatype* data, int idx)` reads a sample identified by `idx`. The data are stored to the struct pointed to by `data`.

The corresponding SD writing function for each data type calls this function and store samples, iterating `idx`, until the function returns `false` when all data has been read.

**Note:** If `idx` is positive, it reads sample number `idx`, counting from the first (i.e. earliest) sample and indexing from zero &mdash; this is used for reading back the entire flash chip, in sequence, on boot. If `idx` is negative, the function reads `idx` samples from the most recent sample, where `idx = -1` corresponds to the most recent sample, `-2` is the sample prior, and so on &mdash; this allows the reading of recent samples, as may be used to integrate IMU data in flight.

#### Step 3: Writing new data

After all data has been saved at power on, the flash chip is cleared.

Space for new data is allocated bsaed on the size and frequency of each data type. For example, the space allocated to a faster sampled data type will be more than for a lower sampled type of the same size.

Memory allocation defines the starting addresses assigned to each type. The `start_addr` value is reset based on this allocation after clearing the chip. The `nSamples` value is reset to 0. 

The start address and number of samples of each type are also stored in the first few bytes of the flash chip.

When a sample is written, `nSamples` is iterated. The address to write data to is calculated based on the number of samples already stored, the size of each sample, and the start address of that data type's allocated storage. 

#### Memory Addresses

The first few bytes store the start address and number of samples. Each of these values uses four bytes, so each data type needs eight bytes. The data are stored in the order `[addr_start,nSamples]`.

We currently store the data structs for IMU, BAROM, ACCEL, and GPS. Therefore, the first 32 bytes of the flash chip stores the start address and size for these data types.

Specifically:

| Data Type | Bytes Storing `addr_start` | Bytes Storing `nSamples` |
| :-------: | :------------------------: | :----------------------: |
|    IMU    |            0-3             |           4-7            |
|   BAROM   |            8-11            |          12-15           |
|   ACCEL   |           16-19            |          20-23           |
|    GPS    |           24-27            |          28-31           |