#ifndef _FilterCreator_h_Digi_
#define _FilterCreator_h_Digi_

#include "TH2.h"
#include "TString.h"
#include <map>
#include <string>
#include "yaml-cpp/yaml.h"

typedef void* (*CreateFilter)();

class FilterCreator
{
public:
    static FilterCreator& GetInstance()
    {
        static FilterCreator Instance;
        return Instance;
    }
    inline void RegistFilter(std::string name, CreateFilter filter) {m_AvaiableFilter[name] = filter;}
    void SetFilter(YAML::Node);
    void* CreateNewFilter();
    inline const YAML::Node& GetYaml() const {return m_FilterConfig;}

    ~FilterCreator();

private:
    FilterCreator();

    std::string m_FilterName;
    YAML::Node m_FilterConfig;
    std::map<std::string, CreateFilter> m_AvaiableFilter;

};

#define REGISTFILTER(name)\
void* CREATE##name(){\
    name* fil = new name();\
    return (fil == nullptr) ? nullptr : fil;\
}\
class REGIST##name\
{\
public:\
    REGIST##name()\
    {\
        FilterCreator::GetInstance().RegistFilter(#name, CREATE##name);\
    }\
};\
REGIST##name regist##name;

#endif