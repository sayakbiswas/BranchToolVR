#ifndef TESTSSETTINGS_H
#define TESTSSETTINGS_H

#include <map>
#include <string>

namespace tests
{

class settings
{

public:

    static settings& getSettings();

    void set(const std::string& optionAndValue);

    const std::string& get(const std::string& optionName) const;

private:
    settings();

    typedef std::map<std::string, std::string> settings_t;
    settings_t m_settings;

};

}

#endif // TESTSSETTINGS_H

