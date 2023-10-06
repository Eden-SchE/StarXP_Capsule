#include "YamlConfig.hh"

void YamlConfig::ReadYamlFile(std::string inputFile)
{
    m_Config = YAML::LoadFile(inputFile);
    m_IfValid = true;
}