#pragma once
#include <string>

class BazowySzyfrator {
public:
    virtual ~BazowySzyfrator() = default;
    virtual std::string szyfruj(std::string tekst) = 0;
    virtual std::string deszyfruj(std::string tekst) = 0;
};