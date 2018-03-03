#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ola/Callback.h>
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/OlaDevice.h>
#include <ola/StringUtils.h>
#include <ola/base/SysExits.h>
#include <ola/client/ClientWrapper.h>
#include <ola/client/OlaClient.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "RpiDmxOutput.hpp"

#ifndef OLATORPIMULTI_H_
#define OLATORPIMULTI_H_

class OlaToRpiMulti {
public:
    ~OlaToRpiMulti();

    int Init();
    int Start();
    void Stop();

private:
    ola::client::OlaClientWrapper m_client;

    // Vector of universe ids since they might not be consecutive
    std::vector<unsigned int> m_universes;

    RpiDmxOutput* rpiOut = nullptr;

    void NewFrame(const ola::client::DMXMetadata &meta,
                  const ola::DmxBuffer &data);
    void UniverseListCallback(const ola::client::Result& result, const std::vector<ola::client::OlaUniverse>& universes);
    void RegisterComplete(const ola::client::Result &result);
};
#endif  // OLATORPIMULTI_H_
