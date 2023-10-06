#include "StringConver.h"

G4double ConvertStrToDouble(std::string Str)
{
    if (!(std::isdigit((Str.c_str()[0])) || (Str.c_str()[0] == '-')))
        throw "ConvertStrToDouble: Input str is not a vaild number!";
    return std::stod(Str);
}