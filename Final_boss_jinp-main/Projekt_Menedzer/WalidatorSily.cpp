#include "WalidatorSily.h"
#include <cctype>
#include <stdexcept>

using namespace std;

bool WalidatorSily::poprawnaDlugosc(const string& h) {
    return h.length() >= 8 && h.length() <= 64;
}

bool WalidatorSily::maDuzaLitere(const string& h) {
    for (char c : h) if (isupper(c)) return true;
    return false;
}

bool WalidatorSily::maMalaLitere(const string& h) {
    for (char c : h) if (islower(c)) return true;
    return false;
}

bool WalidatorSily::maCyfre(const string& h) {
    for (char c : h) if (isdigit(c)) return true;
    return false;
}

bool WalidatorSily::maZnakSpecjalny(const string& h) {
    for (char c : h) if (ispunct(c)) return true;
    return false;
}

bool WalidatorSily::nieMaSpacji(const string& h) {
    for (char c : h) if (isspace(c)) return false;
    return true;
}

bool WalidatorSily::czyJestSilne(const string& haslo) {
    if (haslo.empty()) {
        throw invalid_argument("Haslo nie moze byc puste");
    }

    using FunkcjaTestujaca = bool (*)(const string&);

    FunkcjaTestujaca testy[] = {
        poprawnaDlugosc, 
        maDuzaLitere, 
        maMalaLitere, 
        maCyfre, 
        maZnakSpecjalny, 
        nieMaSpacji
    };

    int spelnioneKryteria = 0;

    for (FunkcjaTestujaca test : testy) {
        if (test(haslo)) {
            spelnioneKryteria++;
        }
    }

    return spelnioneKryteria >= 6;
}