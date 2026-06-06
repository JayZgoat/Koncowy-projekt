#pragma once
#include "BazowyWalidator.h"
#include <string>

class WalidatorSily : public BazowyWalidator {
private:
    static bool poprawnaDlugosc(const std::string& h);
    static bool maDuzaLitere(const std::string& h);
    static bool maMalaLitere(const std::string& h);
    static bool maCyfre(const std::string& h);
    static bool maZnakSpecjalny(const std::string& h);
    static bool nieMaSpacji(const std::string& h);

public:
    bool czyJestSilne(const std::string& haslo) override;
};