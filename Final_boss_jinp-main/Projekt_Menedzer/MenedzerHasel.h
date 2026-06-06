#pragma once
#include <vector>
#include <memory>
#include <string>
#include "WpisHasla.h"
#include "BazowySzyfrator.h"
#include "BazowyWalidator.h"

struct OdszyfrowanyWpis {
    std::string serwis;
    std::string login;
    std::string haslo;
};

class MenedzerHasel {
private:
    std::shared_ptr<BazowySzyfrator> szyfrator;
    std::shared_ptr<BazowyWalidator> walidator;
    std::vector<WpisHasla> listaHasel;

    void wczytajZPliku();

public:
    MenedzerHasel(std::shared_ptr<BazowySzyfrator> s, std::shared_ptr<BazowyWalidator> w);
    bool dodajHaslo(std::string s, std::string l, std::string h);
    std::vector<OdszyfrowanyWpis> pobierzWszystkie();
    void usunHaslo(const std::string& serwis, const std::string& login);
};