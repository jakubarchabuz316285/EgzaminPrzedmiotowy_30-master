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
