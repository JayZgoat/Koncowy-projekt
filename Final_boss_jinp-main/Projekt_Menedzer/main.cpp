#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include "httplib.h"
#include "WalidatorSily.h"
#include "SzyfratorXOR.h"
#include "MenedzerHasel.h"


time_t czas_wygasniecia_sesji = 0; //robimy se do zrobienia konca sesji

bool sprawdzSesje(const httplib::Request& req) {
    if (req.has_header("Cookie")) {
        std::string ciastko = req.get_header_value("Cookie");
        if (ciastko.find("sesja=autoryzowany") != std::string::npos) {

            // Jeśli stoper wybił zero - odrzucamy!
            if (time(nullptr) > czas_wygasniecia_sesji) {
                return false;
            }

            // ODNAWIAMY SESJĘ! Przy każdym poprawnym działaniu resetujemy stoper na 5 minut (300 sekund)
            czas_wygasniecia_sesji = time(nullptr) + 300;
            return true;
        }
    }
    return false;
}

int main() {
    auto walidator = std::make_shared<WalidatorSily>();
    auto szyfrator = std::make_shared<SzyfratorXOR>();
    MenedzerHasel menedzer(szyfrator, walidator);

    httplib::Server serwer;

    // STRONA GŁÓWNA
    serwer.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        if (sprawdzSesje(req)) {
            // PANEL PO ZALOGOWANIU
            std::string html = R"(
<!DOCTYPE html><html lang="pl"><head><meta charset="UTF-8"><title>Menedżer Haseł</title>
<style>
* { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
body { background-color: #121212; color: #e0e0e0; display: flex; justify-content: center; padding: 40px 20px; }
.container { width: 100%; max-width: 800px; background-color: #1e1e1e; border-radius: 12px; box-shadow: 0 8px 24px rgba(0, 0, 0, 0.5); padding: 30px; }
header h1 { text-align: center; color: #4facfe; margin-bottom: 10px; font-size: 28px; letter-spacing: 1px; }
.logout-btn { display: block; text-align: center; margin-bottom: 30px; color: #aaa; text-decoration: none; }
.logout-btn:hover { color: #fff; }
.section-title { border-bottom: 2px solid #333; padding-bottom: 10px; margin-bottom: 20px; color: #ffffff; font-size: 20px; }
form { display: grid; grid-template-columns: 1fr; gap: 15px; margin-bottom: 40px; }
.input-group { display: flex; flex-direction: column; }
label { font-size: 14px; margin-bottom: 5px; color: #aaa; }
input { padding: 12px; border: 1px solid #333; background-color: #2a2a2a; color: #fff; border-radius: 6px; font-size: 16px; transition: border-color 0.3s; }
input:focus { outline: none; border-color: #4facfe; background-color: #333; }
button { background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); color: #000; border: none; padding: 14px; font-size: 16px; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 10px; transition: opacity 0.3s, transform 0.1s; }
button:hover { opacity: 0.9; }
button:active { transform: scale(0.98); }
table { width: 100%; border-collapse: collapse; background-color: #2a2a2a; border-radius: 6px; overflow: hidden; }
th, td { padding: 15px; text-align: left; border-bottom: 1px solid #333; }
th { background-color: #1a1a1a; color: #4facfe; font-weight: 600; }
tr:last-child td { border-bottom: none; }
tr:hover { background-color: #333; }
.empty-state { text-align: center; color: #888; font-style: italic; }
/* NOWE: Style dla przycisku usuwania */
.btn-delete { background: #ff4b4b; color: white; padding: 8px 12px; font-size: 13px; border: none; border-radius: 4px; cursor: pointer; margin: 0; box-shadow: none; }
.btn-delete:hover { opacity: 0.8; box-shadow: 0 0 10px rgba(255, 75, 75, 0.5); background: #ff4b4b;}
</style></head><body>
<div class="container">
    <header>
        <h1>🔒 Menedżer Haseł</h1>
        <a href="/wyloguj" class="logout-btn">Wyloguj się z Sejfu</a>
    </header>
    <main>
        <section>
            <h2 class="section-title">Dodaj Nowe Hasło</h2>
            <form action="/api/dodaj" method="POST">
                <div class="input-group">
                    <label for="witryna">Witryna / URL:</label>
                    <input type="text" id="witryna" name="witryna" placeholder="np. github.com" required>
                </div>
                <div class="input-group">
                    <label for="log">Login / E-mail:</label>
                    <input type="text" id="log" name="log" placeholder="Twój login" required>
                </div>
                <div class="input-group">
                    <label for="haslo">Hasło:</label>
                    <input type="password" id="haslo" name="haslo" placeholder="••••••••" required>
                </div>
                <button type="submit">Zapisz hasło w sejfie</button>
            </form>
        </section>
        <section>
            <h2 class="section-title">Lista Haseł</h2>
            <table><thead><tr><th>Witryna</th><th>Login</th><th>Hasło</th><th>Akcje</th></tr></thead><tbody>
            )";

            auto wszystkieHasla = menedzer.pobierzWszystkie();
            if (wszystkieHasla.empty()) {
                // NOWE: colspan="4" zamiast 3
                html += R"(<tr><td colspan="4" class="empty-state">Brak zapisanych haseł. Dodaj pierwsze!</td></tr>)";
            } else {
                for (const auto& w : wszystkieHasla) {
                    html += "<tr><td>" + w.serwis + "</td><td>" + w.login + "</td><td>" + w.haslo + "</td>";

                    // NOWE: Dodano ukryty formularz z przyciskiem Usuń do każdej komórki "Akcje"
                    html += "<td><form action='/api/usun' method='POST' style='margin:0;'>"
                            "<input type='hidden' name='witryna' value='" + w.serwis + "'>"
                            "<input type='hidden' name='log' value='" + w.login + "'>"
                            "<button type='submit' class='btn-delete'>Usuń</button>"
                            "</form></td></tr>";
                }
            }

            html += R"(</tbody></table></section></main></div></body></html>)";
            res.set_content(html, "text/html");
        }
        else {
            // PANEL LOGOWANIA
            std::string html = R"(
<!DOCTYPE html><html lang="pl"><head><meta charset="UTF-8"><title>Logowanie - Menedżer Haseł</title>
<style>
* { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
body { background-color: #121212; color: #e0e0e0; display: flex; justify-content: center; align-items: center; height: 100vh; padding: 20px; }
.login-container { width: 100%; max-width: 400px; background-color: #1e1e1e; border-radius: 12px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.8); padding: 40px 30px; text-align: center; }
.icon-lock { font-size: 48px; margin-bottom: 15px; background: -webkit-linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
h1 { color: #ffffff; margin-bottom: 10px; font-size: 24px; letter-spacing: 1px; }
p.subtitle { color: #888; font-size: 14px; margin-bottom: 30px; }
form { display: flex; flex-direction: column; gap: 20px; text-align: left; }
.input-group { display: flex; flex-direction: column; }
label { font-size: 13px; margin-bottom: 8px; color: #aaa; font-weight: 600; text-transform: uppercase; }
input { padding: 14px; border: 1px solid #333; background-color: #2a2a2a; color: #fff; border-radius: 6px; font-size: 16px; transition: all 0.3s ease; }
input:focus { outline: none; border-color: #4facfe; background-color: #333; box-shadow: 0 0 8px rgba(79, 172, 254, 0.3); }
button { background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); color: #000; border: none; padding: 16px; font-size: 16px; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 10px; transition: opacity 0.3s, transform 0.1s; }
button:hover { opacity: 0.9; box-shadow: 0 4px 15px rgba(0, 242, 254, 0.4); }
button:active { transform: scale(0.98); }
</style></head><body>
<div class="login-container">
    <div class="icon-lock">🔒</div>
    <h1>Witaj ponownie</h1>
    <p class="subtitle">Zaloguj się do swojego konta, aby odblokować sejf.</p>
    <form action="/logowanie" method="POST">
        <div class="input-group">
            <label for="username">Adres e-mail / Login</label>
            <input type="text" id="username" name="username" placeholder="np. admin" required>
        </div>
        <div class="input-group">
            <label for="master-password">Hasło główne</label>
            <input type="password" id="master-password" name="master-password" placeholder="••••••••" required>
        </div>
        <button type="submit">Zaloguj się</button>
    </form>
</div></body></html>
            )";
            res.set_content(html, "text/html");
        }
    });

    // OBSŁUGA LOGOWANIA I USTAWIENIE KLUCZA
    serwer.Post("/logowanie", [&](const httplib::Request& req, httplib::Response& res) {
        std::string user = req.get_param_value("username");
        std::string pass = req.get_param_value("master-password");

        if (user == "admin" && pass == "haslo123") {
            czas_wygasniecia_sesji = time(nullptr) + 300;
            res.set_header("Set-Cookie", "sesja=autoryzowany; Path=/; HttpOnly; Max-Age=3600");

            auto szyfratorXOR = std::dynamic_pointer_cast<SzyfratorXOR>(szyfrator);
            if (szyfratorXOR) {
                szyfratorXOR->ustawKlucz(pass);
            }
            res.set_redirect("/");
        } else {
            res.set_content("Zly login lub haslo! <a href='/'>Sprobuj ponownie</a>", "text/html");
        }
    });

    // WYLOGOWANIE I CZYSZCZENIE KLUCZA
    serwer.Get("/wyloguj", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Set-Cookie", "sesja=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");

        czas_wygasniecia_sesji = 0;

        auto szyfratorXOR = std::dynamic_pointer_cast<SzyfratorXOR>(szyfrator);
        if (szyfratorXOR) {
            szyfratorXOR->ustawKlucz("");
        }
        res.set_redirect("/");
    });

    // API DODAWANIA HASŁA
    serwer.Post("/api/dodaj", [&](const httplib::Request& req, httplib::Response& res) {
        if (!sprawdzSesje(req)) {
            res.set_redirect("/");
            return;
        }

        std::string serwis = req.get_param_value("witryna");
        std::string login = req.get_param_value("log");
        std::string haslo = req.get_param_value("haslo");

        if (!walidator->czyJestSilne(haslo)) {
            std::string html_blad = R"(
<!DOCTYPE html><html lang="pl"><head><meta charset="UTF-8"><title>Błąd Walidacji</title>
<style>
* { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
body { background-color: #121212; color: #e0e0e0; display: flex; justify-content: center; align-items: center; height: 100vh; padding: 20px; }
.error-container { width: 100%; max-width: 500px; background-color: #1e1e1e; border-radius: 12px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.8); padding: 40px 30px; text-align: center; border-top: 4px solid #ff4b4b; }
.icon-error { font-size: 48px; margin-bottom: 15px; }
h1 { color: #ff4b4b; margin-bottom: 15px; font-size: 24px; letter-spacing: 1px; }
p { color: #aaa; font-size: 15px; margin-bottom: 30px; line-height: 1.5; }
a.button { display: inline-block; background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); color: #000; text-decoration: none; padding: 14px 24px; font-size: 16px; font-weight: bold; border-radius: 6px; cursor: pointer; transition: opacity 0.3s, transform 0.1s; }
a.button:hover { opacity: 0.9; box-shadow: 0 4px 15px rgba(0, 242, 254, 0.4); }
a.button:active { transform: scale(0.98); }
</style></head><body>
<div class="error-container">
    <div class="icon-error">⚠️</div>
    <h1>Odmowa Dostępu</h1>
    <p>Podane hasło jest zbyt słabe!<br>Aby bezpiecznie zamknąć sejf, upewnij się, że nowe hasło ma co najmniej 8 znaków, zawiera wielką i małą literę, cyfrę oraz znak specjalny.</p>
    <a href="/" class="button">Wróć i spróbuj ponownie</a>
</div></body></html>
            )";
            res.status = 400;
            res.set_content(html_blad, "text/html");
            return;
        }

        menedzer.dodajHaslo(serwis, login, haslo);
        res.set_redirect("/");
    });

    // API USUWANIA HASŁA
    serwer.Post("/api/usun", [&](const httplib::Request& req, httplib::Response& res) {
        if (!sprawdzSesje(req)) {
            res.set_redirect("/");
            return;
        }

        std::string serwis = req.get_param_value("witryna");
        std::string login = req.get_param_value("log");

        menedzer.usunHaslo(serwis, login);

        res.set_redirect("/");
    });

    std::cout << "-------------------------------------------\n";
    std::cout << " SERWER MENEDZERA HASEL DZIALA PRAWIDLOWO! \n";
    std::cout << " Otworz przegladarke: http://localhost:8080 \n";
    std::cout << "-------------------------------------------\n";
    serwer.listen("localhost", 8080);

    return 0;
}