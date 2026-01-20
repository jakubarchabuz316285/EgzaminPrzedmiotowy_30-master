#include "W_USLUG\program.h"
#include "W_DANYCH\importerBlokowy.h"
#include <QDebug>

// KONSTRUKTORY I SETUP:

// W konstruktorze podajemy katalog docelowy, w którym importer
// ma szukać przedmiotów i pytań:
Program::Program(QObject *parent)
    : QObject{parent},m_BazaPytan(new ImporterBlokowy(this,"PytaniaNEW"))
{}

// METODY - USŁUGI:

// USŁUGA zlecająca wybór przedmiotu do bazy pytań (importera):
void Program::setPrzedmiot(QString nazwa)
{
    m_BazaPytan->setPrzedmiot(nazwa);
}

// USŁUGA zlecajaca zadanie wczytania pytań dla aktualnie wybranego przedmiotu.
// Następnie metoda parametryzuje instancje urn do losowania indeksów pytań.
void Program::wczytajPytania()
{   
    m_BazaPytan->wczytajDane();
    int numBlok = m_BazaPytan->getNumBlok();
    for (int blok = 0; blok < numBlok; blok++)
        m_urnyDoLosowania[blok].setLiczElem(m_BazaPytan->getBlok(blok).size());
}

// USŁUGA "forwardująca" liczbę bloków zapisaną w bazie:
int Program::getLiczbaBlokow()
{
    return m_BazaPytan->getNumBlok();
}

// USŁUGA losująca pytania dla danego bloku. Przyjmuje liczbę dodatkowych pytań, gdyż
// zawsze losuje minimum 3 pytania (obowiązkowe na egzaminie):
void Program::losujPytania(int blok, int num)
{
    Q_ASSERT_X(m_urnyDoLosowania[blok].maElementy(),"Program/losujPytania","Niezainicjalizowanoa urna");

    // Wyznaczenie liczby pytań do losowania i przygotowanie vectora buforującego wylosowane pytania:
    int toLos = MIN_LICZ_PYT + num;
    m_WylosPyt[blok].clear();
    m_WylosPyt[blok].resize(toLos);

    // Losowanie indeksów pytań z redukcją powtórzeń (patrz urna.cpp).
    // Istotne jest to że urny losuja tylko indeksy pytań:
    QVector<int> numery = m_urnyDoLosowania[blok].losujRedukcjaPowt(toLos);

    // Pobranie pytań z bazy zgodnie z wylosowanymi indeksami:
    for(int pyt = 0; pyt < toLos; pyt++)
        m_WylosPyt[blok][pyt] = m_BazaPytan->getBlok(blok).at(numery[pyt]);
    QVector<Pytanie> odrzucone;
    historia.dodaj_egzamin(getAktPrzedmiot(), m_WylosPyt[blok], odrzucone);
    // Zlecenie wypisania pytań do warstwy prezentacji przy pomocy sygnału:
    emit wypisz(m_WylosPyt[blok], blok);

}

// // USŁUGA "forwardująca" aktualnie obsługiwany przedmiot (zapisany w bazie)
QString Program::getAktPrzedmiot()
{
    return m_BazaPytan->getPrzedmiot();
}

// // USŁUGA "forwardująca" aktualną listę przedmiotów znalezioną w katalogu z bazą pytań
QStringList Program::getListaPrzedmiotow()
{
    return m_BazaPytan->getListaPrzedmiotow();
}

// USŁUGA oczyszczania wektora wylosowanych pytań z wybranych do usunięcia:
void Program::odznaczPytania(QVector<int> pytDoOdznaczenia, int blok)
{
    // Najpierw sortowane są numery pytań do usuniecia aby uzyskać spójny i uprządkowany zbiór:
    std::sort(pytDoOdznaczenia.rbegin(), pytDoOdznaczenia.rend());

    // Eliminacja kolejnych pytań z wektora pytań (wektor pytań nie jest posortowany -  tak ma być)
    for(auto idx:pytDoOdznaczenia)
        m_WylosPyt[blok].erase(m_WylosPyt[blok].begin()+idx);

    // Ponowne zlecenie wyświetlenia pytań do warstwy prezentacji (tym razem po usunięciu niechcianych):
    emit wypisz(m_WylosPyt[blok], blok);
}

// USŁUGA ponownego wypisania pytań na potrzeby zmiany ich układu (rozbite/normalne):
void Program::ponownieWypiszWylosowane(int blok)
{
    Q_ASSERT_X(blok < MAX_LICZ_BLOKOW,"Program/ponownieWypiszWylosowane","Nieprawidlowy numer bloku");
    // Ponowne zlecenie działa tylko jesli są wylosowane pytania i jest ich już dokłądnie 3. W innych
    // przypadkach ta usługa jest ignorowana.
    if(m_WylosPyt[blok].size() == MIN_LICZ_PYT)
        emit wypisz(m_WylosPyt[blok], blok);
    else return;
    // Powyżej celowo zaznaczono pominięcie wypisywania, choć else jest w tym przypadku
    // zbędny. Chodzi o wyraźne zaznaczenie, że w razie niewłaściwej liczby pytań ta usługa ma
    // nie robić nic!
}
/**
 * Wywołuje proces zapisu aktualnej historii do pliku binarnego.
 * Jest to "pośrednik" między interfejsem użytkownika a klasą Historia_egzaminow.
 */
void Program::zapiszHistorieWPliku()
{
    // Próba zapisu - metoda zapisz_instancje() tworzy plik .bin z aktualną datą
    if(!historia.zapisz_instancje()) {
        qDebug() << "Błąd zapisu instancji w warstwie danych!";
    }
}

/**
 * Wczytuje dane z pliku binarnego i przekształca je w sformatowany dokument HTML.
 * Pozwala na wyświetlenie historii w czytelnej formie (np. w kontrolce QTextBrowser).
 */
QString Program::generujRaportHistorii(const QString &sciezka)
{
    // 1. Próba wczytania danych binarnych z podanej ścieżki
    if (!historia.wczytaj_instancje(sciezka)) {
        return "<h3>Błąd: Nie znaleziono pliku lub plik jest uszkodzony.</h3>";
    }

    // 2. Pobranie wczytanej listy sesji
    const QVector<Historia_egzaminow::Sesja_egzaminacyjna> &sesje = historia.get_historia();

    // 3. Sprawdzenie, czy historia nie jest pusta
    if (sesje.isEmpty()) {
        return "<h3>Brak zapisanych egzaminów w tym pliku.</h3>";
    }

    // 4. Budowanie dokumentu HTML - nagłówek
    QString html = "<html><body><h1 style='color: #2980b9;'>Historia Egzaminów</h1>";

    // 5. Iteracja po każdej sesji egzaminacyjnej w historii
    for (const auto &sesja : sesje) {
        html += "<hr></hr>"; // Linia oddzielająca sesje
        html += "<div style='border: 1px solid #ccc; margin-bottom: 20px; padding: 10px; border-radius: 5px;'>";

        // Wyświetlanie metadanych sesji (nazwa przedmiotu i czas)
        html += "<p><b>Przedmiot:</b> " + sesja.przedmiot + "</p>";
        html += "<p><b>Data:</b> " + sesja.data.toString("dd.MM.yyyy HH:mm") + "</p>";

        // 6. Lista pytań, które zostały wylosowane podczas tej sesji
        html += "<b>Pytania wylosowane:</b><ul>";
        for (const auto &dane : sesja.wylosowane) {
            // Zabezpieczenie na wypadek, gdyby treść pytania była pusta (np. błąd importu)
            QString tekst = dane.tresc.isEmpty() ? "Brak treści (tylko nr: " + QString::number(dane.numer) + ")" : dane.tresc;
            html += "<li>" + tekst + "</li>";
        }
        html += "</ul></div>";
    }

    html += "</body></html>";
    return html; // Zwrócenie gotowego kodu HTML do wyświetlenia
}
