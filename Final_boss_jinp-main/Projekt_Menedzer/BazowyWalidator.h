#pragma once
#include <string>

class BazowyWalidator {
public:
    virtual ~BazowyWalidator() = default;
    virtual bool czyJestSilne(const std::string& haslo) = 0;
};