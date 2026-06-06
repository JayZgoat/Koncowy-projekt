#pragma once
#include "BazowySzyfrator.h"
#include <string>

class SzyfratorXOR : public BazowySzyfrator {
private:
    std::string klucz;
public:
    SzyfratorXOR();
    void ustawKlucz(std::string nowyKlucz);
    std::string szyfruj(std::string tekst) override;
    std::string deszyfruj(std::string tekst) override;
};