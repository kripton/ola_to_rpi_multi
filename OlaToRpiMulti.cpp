#include "OlaToRpiMulti.hpp"

using ola::DmxBuffer;
using ola::client::Result;
using std::string;
using std::vector;

OlaToRpiMulti::~OlaToRpiMulti()
{
}

int OlaToRpiMulti::Init()
{
    if (!m_client.Setup()) {
        std::cout << "Client Setup failed" << std::endl;;
        return ola::EXIT_UNAVAILABLE;
    }
    std::cout << "Client Setup finished" << std::endl;

    m_client.GetClient()->SetDMXCallback(ola::NewCallback(this, &OlaToRpiMulti::NewFrame));
    m_client.GetClient()->FetchUniverseList(ola::NewSingleCallback(this, &OlaToRpiMulti::UniverseListCallback));

    return ola::EXIT_OK;
}

int OlaToRpiMulti::Start()
{
    m_client.GetSelectServer()->Run();
    return ola::EXIT_OK;
}

void OlaToRpiMulti::Stop()
{
    if (rpiOut != nullptr) {
        rpiOut->Stop();
    }
    m_client.GetSelectServer()->Terminate();
}


void OlaToRpiMulti::NewFrame(const ola::client::DMXMetadata &meta, const ola::DmxBuffer &data)
{
    std::vector<unsigned int>::iterator it;
    unsigned int index = UINT_MAX;

    //std::cout << "NewFrame! Universe Id: " << meta.universe << " Size: " << data.Size() << std::endl;

    // Get the "index" of the universe we only got the id from
    it = std::find (m_universes.begin(), m_universes.end(), meta.universe);
    if (it != m_universes.end()) {
        //std::cout << "Element found in myvector: " << *it << " " << std::distance(m_universes.begin(), it) << std::endl;
        index = std::distance(m_universes.begin(), it);
    }
    else
    {
        std::cout << "Universe id " << meta.universe << " not found in list of universes :-O" << std::endl;
    }

    // Let the rpiOut code copy the data to where it needs it
    if ((index != UINT_MAX) && (rpiOut != nullptr))
    {
        rpiOut->SetDmxData(index, data.GetRaw(), data.Size());
    }
}

void OlaToRpiMulti::UniverseListCallback(const ola::client::Result& result, const std::vector<ola::client::OlaUniverse>& universes)
{
    if (result.Success()) {
        std::cout << "UNIVERSES: " << universes.size() << std::endl;

        for (size_t i = 0; i < universes.size(); i++) {
            std::cout << "\tID: " << universes[i].Id() << " Name: \"" << universes[i].Name() << "\"" << std::endl;
            m_client.GetClient()->RegisterUniverse(universes[i].Id(), ola::client::REGISTER, ola::NewSingleCallback(this, &OlaToRpiMulti::RegisterComplete));
            m_universes.push_back(universes[i].Id());
        }

        // Initialize the dmx output
        rpiOut = new RpiDmxOutput(universes.size());
        rpiOut->Start();

        // TODO Fetch initial values of each universe we registered for
        //      instead of waiting for first change

    } else {
        std::cout << "Unable to get list of universes :(" << std::endl;
    }
}


void OlaToRpiMulti::RegisterComplete(const Result &result)
{
    if (!result.Success()) {
        std::cout << "Register failed" << std::endl;
    } else {
        std::cout << "Register completed" << std::endl;
    }
}
