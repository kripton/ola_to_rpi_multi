#include "RpiDmxOutput.hpp"

RpiDmxOutput::RpiDmxOutput(unsigned int numberOfUniverses)
{
    std::cout << "RpiDmxOutput INIT! PIGPIO version: " << PIGPIO_VERSION << std::endl;

    pigpiodHandle = pigpio_start(NULL, NULL);
    std::cout << "pigpio_start() returned: " << pigpiodHandle << std::endl;

    if (MAX_UNIVERSES < numberOfUniverses)
    {
        std::cout << "OLA currently has " << numberOfUniverses << " but we only support " << MAX_UNIVERSES << ". Truncated" << std::endl;
        numberOfUniverses = MAX_UNIVERSES;
    }

    numUniverses = numberOfUniverses;

    for (unsigned int i = 0; i < numberOfUniverses; i++)
    {
        std::cout << "Configuring GPIO " << GPIOs[i] << " for universe " << i << std::endl;
        set_mode(pigpiodHandle, GPIOs[i], PI_OUTPUT);
    }

    if (driverEnable)
    {
        std::cout << "Driver-Enable output ACTIVE on GPIO " << driverEnable << std::endl;
        set_mode(pigpiodHandle, driverEnable, PI_OUTPUT);
    }
}

void RpiDmxOutput::SetDmxData(unsigned int universeIndex, const uint8_t *data, const unsigned int size)
{
    // memcopy the data from the given buffer into our internal buffer
    unsigned int copySize = MIN(512, size);

    std::memcpy(DmxBuffers[universeIndex], data, copySize);
}

void RpiDmxOutput::Start()
{
    // Zero the DmxBuffers
    std::memset(DmxBuffers, 0, MAX_UNIVERSES * 512);

    shallRun = true;
    workerThread = std::thread(&RpiDmxOutput::Run, this);
}

void RpiDmxOutput::Stop()
{
    shallRun = false;

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    pigpio_stop(pigpiodHandle);
}

void RpiDmxOutput::Run()
{
    // Number of bits to be sent
    int numbits = 0;

    std::cout << "RpiDmxOutput::Run() starting :D" << std::endl;
    while (shallRun)
    {
        // Generate the wave table
        numbits = buildDmxPacket();

        // Send it
        sendDmxPacket(numbits);
    }
    std::cout << "RpiDmxOutput::Run() ending :(" << std::endl;
}

void RpiDmxOutput::outputLow(int *idx, int GPIO, int duration_us)
{
    if (firstUni)
    {
        pulse[*idx].gpioOn = (1<<GPIO);
        pulse[*idx].gpioOff = 0;
        pulse[*idx].usDelay = duration_us;
    }
    else
    {
        pulse[*idx].gpioOn |= (1<<GPIO);
    }
    *idx=*idx+1;
}

void RpiDmxOutput::outputHigh(int *idx, int GPIO, int duration_us)
{
    if (firstUni)
    {
        pulse[*idx].gpioOn = 0;
        pulse[*idx].gpioOff = (1<<GPIO);
        pulse[*idx].usDelay = duration_us;
    }
    else
    {
        pulse[*idx].gpioOff |= (1<<GPIO);
    }
    *idx=*idx+1;
}

// Output 1 start bit + 8 data bit + 2 stop bits
void RpiDmxOutput::outputSerialbyte(int *pidx, int GPIO, uint8_t c)
{
    int b = 0;

    // 1 start bit
    outputLow(pidx, GPIO, SR);

    // 8 data bits
    for (b=0; b < 8; b++)
    {
        if (c & (1<<b))
        {
            outputHigh(pidx, GPIO, SR);
        }
        else
        {
            outputLow(pidx, GPIO,SR);
        }
    }

    // 2 stop bits
    outputHigh(pidx, GPIO, SR*2);
}

int RpiDmxOutput::buildDmxPacket()
{
    int ch;
    int pidx = 0;
    int allNullFrame = 1;
    int allFullFrame = 1;

    firstUni = true;

    // For every universe
    for (unsigned int uni = 0; uni< numUniverses; uni++)
    {
        allNullFrame = 1;
        allFullFrame = 1;
        //std::cout << "buildDmxPacket. numUniverses: " << numUniverses << " Univ: " << uni << std::endl;

        // We have to reset this so we send all universes AT ONCE
        // and not one after the other
        // Since they will all create the same number of bits this
        // should not be a problem
        pidx = 0;

        // If driver-enable output is to be used, enable the driver first
        if (driverEnable)
        {
            outputHigh(&pidx, driverEnable, SR);
        }

        // PRE packet idle time
#ifdef PREPACKET_IDLE_US
        output_high(&pidx, GPIOs[uni], PREPACKET_IDLE_US);
#endif

        // BREAK
        outputLow(&pidx, GPIOs[uni], BREAK_US);

        // MAB (= Mark after break)
        outputHigh(&pidx, GPIOs[uni],MAB_US);

        // Start code = 0
        outputSerialbyte(&pidx, GPIOs[uni], 0);

        // Iterate over the channels of this universe
        for (ch = 0; ch < 512; ch++)
        {
            if (DmxBuffers[uni][ch] != 0) {
                allNullFrame = 0;
            }
            if (DmxBuffers[uni][ch] != 255) {
                allFullFrame = 0;
            }
            outputSerialbyte(&pidx, GPIOs[uni], DmxBuffers[uni][ch]);
        }

        if (allNullFrame) {
            std::cout << "All null frame on universe " << uni << std::endl;
        }
        if (allFullFrame) {
            std::cout << "All FULL frame on universe " << uni << std::endl;
        }

        // Idle high for POSTPACKET_IDLE_US
        outputHigh(&pidx, GPIOs[uni], POSTPACKET_IDLE_US);

	// Bring output LOW until next BREAK starts
	outputLow(&pidx, GPIOs[uni], 4);

        // Release the driver-enable output
        if (driverEnable)
        {
            outputLow(&pidx, driverEnable, SR);
        }

        firstUni = false;
    }

    return(pidx);
}

void RpiDmxOutput::sendDmxPacket(int numbits)
{
    int wave_id=0;

    wave_add_generic(pigpiodHandle, numbits, pulse);

    wave_id = wave_create(pigpiodHandle);

    // Send this bit pattern once via DMA IO
    wave_send_once(pigpiodHandle, wave_id);

    // Poll for DMA IO to complete
    while (wave_tx_busy(pigpiodHandle) == 1)
    {
        // yield to kernel while DMA is in progress
        usleep(500);
    }

    // finished with this pattern
    wave_delete(pigpiodHandle, wave_id);
}
