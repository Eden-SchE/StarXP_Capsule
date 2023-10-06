// --------------------------------------------------------------
//      Star-XP Digitize Framework
//
// --------------------------------------------------------------

#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <sys/stat.h>
#include <iostream>

#include "TSystem.h"
#include "TString.h"
#include "TEnv.h"
#include "TRandom.h"

#include "TimeControl.hh"
#include "YamlConfig.hh"

DigitizationConfig ConfigDigi;

void PrintUsage()
{
    std::cout << "****************************************************************" << std::endl;
    std::cout << "****************************************************************" << std::endl;
    std::cout << "********** Welcome to Star-XP Digitization Framework! **********" << std::endl;
    std::cout << "********** Usage: ./Digitize [DigiCard.card]          **********" << std::endl;
    std::cout << "****************************************************************" << std::endl;
    std::cout << "****************************************************************" << std::endl;
}

bool IfFileExist(TString FilePath)
{
    struct stat buffer;   
    return (stat(FilePath.Data(), &buffer) == 0);
}

std::vector<std::string> SplitStr(const char* Str, const char* Split)
{
    std::string VStr(Str);
    std::vector<std::string> Vector;
    std::regex Delimiter(Split);
    std::sregex_token_iterator iter(VStr.begin(), VStr.end(), Delimiter, -1);
    std::sregex_token_iterator end;
    while (iter != end)
    {   
        Vector.push_back(std::string(*iter));
        ++iter;
    }
    return Vector;
}

std::vector<double> ConvertStrToVectordouble(const char* Str)
{
    std::vector<double> VectorDouble;
    VectorDouble.clear();
    if(Str == "")
        return VectorDouble;
    std::vector<std::string> StrVector = SplitStr(Str, ",");
    for(auto str:StrVector)
    {
        if(!(std::isdigit((str.c_str()[0])) || (str.c_str()[0] == '-')))
            throw "ConvertStrToVectordouble: Input str is not a vaild number!";
        VectorDouble.push_back(std::stod(str));
    }
    return VectorDouble;
}

//main function
int main(int argc, char **argv)
{
    std::string InitCard = "DigiCard.yml";
    bool UseDefaultCard = true;
    // Add arglist control
    PrintUsage();
    if (argc >= 2)
    {
        for(int iA = 1; iA < argc; ++iA)
        {
            if(SplitStr(argv[iA], "\\.").back() == std::string("yml"))
            {
                InitCard = TString(argv[iA]);
                UseDefaultCard = false;
            }
        }
    }
    if(UseDefaultCard)
    {
        std::cout << "Warning!!! Using default " << InitCard << std::endl;
        if (!IfFileExist(InitCard))
        {
            std::cerr << "Error!!! Initialize card \" " << InitCard << " \" NOT FOUND!!!" << std::endl;
            return 1;
        }
    }

    //TimeControl Initialization
    YamlConfig::GetInstance().ReadYamlFile(InitCard);
    gRandom->SetSeed(YamlConfig::GetInstance().GetYaml()["RandomSeed"].as<int>(2022));

    TimeControl::GetInstance().Process();
    std::cout << "ALL DONE." << std::endl;
    std::cout << "Enjoy your analysis!" << std::endl;
}
