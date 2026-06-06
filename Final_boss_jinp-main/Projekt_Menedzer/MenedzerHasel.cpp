#include "MenedzerHasel.h"
#include <fstream>
#include <iostream>

MenedzerHasel::MenedzerHasel(std::shared_ptr<BazowySzyfrator> s, std::shared_ptr<BazowyWalidator> w) 
    : szyfrator(s), walidator(w) {
    wczytajZPliku();
}

void MenedzerHasel::wczytajZPliku() {
    std::ifstream plik("baza_hasel.txt");
    std::string s, l, h;
    
    while (plik >> s >> l >> h) {
        listaHasel.push_back({s, l, h});
    }
    plik.close();
}

bool MenedzerHasel::dodajHaslo(std::string s, std::string l, std::string h) {
    if (walidator->czyJestSilne(h)) {
        WpisHasla nowyWpis;
        nowyWpis.serwis = s;
        nowyWpis.login = l;
        nowyWpis.zaszyfrowaneHaslo = szyfrator->szyfruj(h);

        listaHasel.push_back(nowyWpis);

        std::ofstream plik("baza_hasel.txt", std::ios::app);
        plik << nowyWpis.serwis << " " << nowyWpis.login << " " << nowyWpis.zaszyfrowaneHaslo << "\n";
        plik.close();

        return true;
    }
    return false;
}

std::vector<OdszyfrowanyWpis> MenedzerHasel::pobierzWszystkie() {
    std::vector<OdszyfrowanyWpis> wynik;
    for (const auto& wpis : listaHasel) {
        wynik.push_back({wpis.serwis, wpis.login, szyfrator->deszyfruj(wpis.zaszyfrowaneHaslo)});
    }
    return wynik;
}

void MenedzerHasel::usunHaslo(const std::string& serwis, const std::string& login) {
    // 1. Szukamy wpisu w pamięci RAM i go usuwamy
    for (auto it = listaHasel.begin(); it != listaHasel.end(); ++it) {
        if (it->serwis == serwis && it->login == login) {
            listaHasel.erase(it);
            break; // Znaleziono i usunięto, przerywamy pętlę
        }
    }

    // 2. Otwieramy plik (bez 'app', co sprawi, że plik się wyczyści)
    // i zapisujemy aktualny stan listy na nowo
    std::ofstream plik("baza_hasel.txt");
    if (plik.is_open()) {
        for (const auto& wpis : listaHasel) {
            // Zapisujemy w takiej samej formie, w jakiej wczytujemy
            plik << wpis.serwis << " " << wpis.login << " " << wpis.zaszyfrowaneHaslo << "\n";
        }
        plik.close();
    }
}