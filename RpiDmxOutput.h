#ifndef RPIDMXOUTPUT_H
#define RPIDMXOUTPUT_H

#include <stdint.h>
#include <unistd.h>
#include <sys/param.h>
#include <thread>
#include <cstring>
#include <iostream>
#include <string>

#include <pigpio.h>

// Maximum number of universes this class supports
#define MAX_UNIVERSES       10

// Large enough to hold the DMX512 packet in bit form
#define NUM_RAWBITS         6000

// Timings
#define BREAK_US            120
#define MAB_US              12
//#define PREPACKET_IDLE_US   20  // comment out to disable
#define POSTPACKET_IDLE_US  50
#define SR                  4   // symbol rate, 4us per bit


class RpiDmxOutput
{
public:
    RpiDmxOutput(unsigned int numberOfUniverses);

    void SetDmxData(unsigned int universeIndex, const uint8_t *data, const unsigned int size);

    void Start();
    void Stop();

private:
    // Actual number of universes in use
    unsigned int numUniverses = 0;

    // We provide 10 GPIOs here, from lowest to highest
    // Sparing out: 2,3 (I2C); 9,10 (MISO, MOSI); 14,15 (UART); 27 (trigger
    // helper, see below)
    // If you'd like to spare SCLK, CE0 and CE1 OR not use the trigger helper
    // in order to output more universes, change the array and MAX_UNIVERSES
    // Of course you can also use more GPIOs on RPi models featuring more GPIOs
    const int GPIOs[MAX_UNIVERSES] = {4, 7, 8, 11, 17, 18, 22, 23, 24, 25}; //, 27};

    // Optional oscilloscope trigger helper: Short burst during BREAK to
    // trigger an oscilloscope to the start of the DMX frames
    // Set to 0 to disable
    int triggerHelper = 27;

    // Actual DMX values for as many universes as we support
    uint8_t DmxBuffers[MAX_UNIVERSES][512];

    // The awesome GPIO wave table
    gpioPulse_t pulse[NUM_RAWBITS];

    // Main worker function and exit condition
    void Run();
    bool shallRun = true;

    std::thread workerThread;

    // Helper var to make sure we correctly initialize the wave table with the first universe
    bool firstUni = true;

    // Helper functions to generate the wave table from the DmxBuffer
    void outputLow(int *idx, int GPIO, int duration_us);
    void outputHigh(int *idx, int GPIO, int duration_us);
    void outputSerialbyte(int *pidx, int GPIO, uint8_t c);
    int buildDmxPacket();
    void sendDmxPacket(int numbits);
};

#endif // RPIDMXOUTPUT_H
