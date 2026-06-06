#include "SzyfratorXOR.h"
#include <stdexcept>

using namespace std;

SzyfratorXOR::SzyfratorXOR() : klucz("") {}

void SzyfratorXOR::ustawKlucz(string nowyKlucz) {
    klucz = nowyKlucz;
}

string SzyfratorXOR::szyfruj(string tekst) {
    if (tekst.empty()) {
        throw invalid_argument("Szyfrator: Tekst do zaszyfrowania nie moze byc pusty!");
    }
    if (klucz.empty()) {
        return "BLAD_BRAK_KLUCZA"; 
    }

    string wynik = tekst;
    for (int i = 0; i < wynik.size(); i++) {
        wynik[i] = wynik[i] ^ klucz[i % klucz.length()];
    }
    return wynik;
}

string SzyfratorXOR::deszyfruj(string tekst) {
    if (tekst == "BLAD_BRAK_KLUCZA") return tekst;
    return szyfruj(tekst); // XOR dziala w obie strony tak samo
}