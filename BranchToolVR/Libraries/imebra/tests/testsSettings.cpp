#include "testsSettings.h"

namespace tests
{


settings::settings()
{

}

settings& settings::getSettings()
{
    static settings settingsInstance;
    return settingsInstance;
}


void settings::set(const std::string& optionAndValue)
{
    size_t equalSign = optionAndValue.find('=');
    if(equalSign == std::string::npos)
    {
        m_settings[optionAndValue] = "1";
        return;
    }
    std::string option = optionAndValue.substr(0, equalSign);
    std::string value = optionAndValue.substr(++equalSign);

    m_settings[option] = value;
}

const std::string& settings::get(const std::string& optionName) const
{
    static const std::string empty;
    settings_t::const_iterator findOption = m_settings.find(optionName);
    if(findOption == m_settings.end())
    {
        return empty;
    }
    return findOption->second;
}

}

