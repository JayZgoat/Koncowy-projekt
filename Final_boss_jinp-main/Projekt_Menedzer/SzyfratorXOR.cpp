#include "SzyfratorXOR.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;

SzyfratorXOR::SzyfratorXOR() : klucz("") {}

void SzyfratorXOR::ustawKlucz(string nowyKlucz) {
    klucz = nowyKlucz;
}

string naHex(const string& wejscie) {
    stringstream ss;
    for (unsigned char c : wejscie) {
        ss << hex << setw(2) << setfill('0') << (int)c;
    }
    return ss.str();
}

string zHex(const string& wejscie) {
    string wyjscie;
    // Zabezpieczenie: jeśli wejście jest puste, nie robimy nic
    if (wejscie.empty() || wejscie == "BLAD_BRAK_KLUCZA") return "";

    // Zmiana warunku na i + 1 < wejscie.length(), aby mieć pewność,
    // że zawsze mamy co najmniej 2 znaki do odczytania przez substr.
    for (size_t i = 0; i + 1 < wejscie.length(); i += 2) {
        string bajt = wejscie.substr(i, 2);
        char znak = (char)(int)strtol(bajt.c_str(), nullptr, 16);
        wyjscie.push_back(znak);
    }
    return wyjscie;
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

    return naHex(wynik);
}

string SzyfratorXOR::deszyfruj(string tekst) {
    if (tekst == "BLAD_BRAK_KLUCZA") return tekst;
    // Zabezpieczenie przed próbą deszyfrowania z pustym kluczem
    if (klucz.empty()) return "BLAD_BRAK_KLUCZA";

    string zdekodowanyHex = zHex(tekst);

    string wynik = zdekodowanyHex;
    for (int i = 0; i < wynik.size(); i++) {
        wynik[i] = wynik[i] ^ klucz[i % klucz.length()];
    }
    return wynik;
}