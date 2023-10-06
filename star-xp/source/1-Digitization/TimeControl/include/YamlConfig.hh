#ifndef YamlConfig_h_Digi_
#define YamlConfig_h_Digi_

#include "yaml-cpp/yaml.h"
#include <string>

class YamlConfig
{
public:
    YamlConfig(){ m_IfValid = false; }
    ~YamlConfig(){}
    static YamlConfig& GetInstance()
    {
        static YamlConfig Instance;
        return Instance;
    }
    void ReadYamlFile(std::string);
    inline bool IfValid() const {return m_IfValid;}
    inline const YAML::Node& GetYaml() const {return m_Config;}

private:
    YAML::Node m_Config;
    bool m_IfValid;
};

#endif