#include "FilterCreator.hh"
#include <iostream>

FilterCreator::FilterCreator()
{
    m_FilterName = "SaveAllFilter";
}

FilterCreator::~FilterCreator(){}

void FilterCreator::SetFilter(YAML::Node FilterNode)
{
    std::string name = FilterNode["Type"].as<std::string>();
    if (m_AvaiableFilter.find(name) != m_AvaiableFilter.end())
    {
        m_FilterName = name;
        if(FilterNode["Config"].IsDefined())
            m_FilterConfig = FilterNode["Config"];
    }
    else
    {
        std::cout << "Warning!!! Filter Type \" " << name << " \" NOT FOUND!!! Use default Filter \" SaveAll \"." << std::endl;
        std::cout << "Avaiable Type:" << std::endl;
        for(const auto& FPtr:m_AvaiableFilter)
            std::cout << "  " << FPtr.first << std::endl;
        m_FilterName = "SaveAllFilter";
    }
}

void* FilterCreator::CreateNewFilter()
{
    auto iter = m_AvaiableFilter.find(m_FilterName);
    CreateFilter filter = iter->second;
    return filter();
}


