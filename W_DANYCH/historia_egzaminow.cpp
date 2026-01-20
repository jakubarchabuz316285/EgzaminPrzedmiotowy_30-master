#include "historia_egzaminow.h"

    /**
 * Dodaje nową sesję egzaminacyjną do historii.
 * Przepisuje dane z obiektów klasy Pytanie do uproszczonej struktury zapisu.
 */
    void Historia_egzaminow::dodaj_egzamin(const QString &przedmiot, const QVector<Pytanie> &wylosowane_p, const QVector<Pytanie> &odrzucone_p)
{
    Sesja_egzaminacyjna sesja;
    sesja.przedmiot = przedmiot;
    sesja.data = QDateTime::currentDateTime(); // Rejestracja aktualnego czasu egzaminu

    // Konwersja pełnych obiektów Pytanie na mniejsze struktury (numer, blok, treść)
    for (const Pytanie &p : wylosowane_p)
        sesja.wylosowane.append({p.getNumer(), p.getBlok(), p.getCzystyNaglowek()});

    for (const Pytanie &p : odrzucone_p)
        sesja.odrzucone.append({p.getNumer(), p.getBlok(), p.getCzystyNaglowek()});

    // Dodanie gotowej sesji do kontenera przechowującego całą historię
    lista_egzaminow.append(sesja);
}

/**
 * Zwraca referencję do pełnej listy zapisanych sesji.
 */
const QVector<Historia_egzaminow::Sesja_egzaminacyjna> &Historia_egzaminow::get_historia() const
{
    return lista_egzaminow;
}

/**
 * Usuwa wszystkie wpisy z historii egzaminów.
 */
void Historia_egzaminow::wyczysc()
{
    lista_egzaminow.clear();
}

/**
 * Ustawia ścieżkę do pliku, w którym ma być przechowywana historia.
 */
void Historia_egzaminow::set_sciezka(const QString &sciezka)
{
    sciezka_zapisu = sciezka;
}

/**
 * Zapisuje aktualny stan historii do pliku binarnego.
 * Nazwa pliku generowana jest automatycznie na podstawie aktualnej daty.
 */
bool Historia_egzaminow::zapisz_instancje() const
{
    // Tworzenie nazwy pliku np. "raport_20231027_123000.bin"
    QString sciezka = QDateTime::currentDateTime().toString("raport_yyyyMMdd_hhmmss") + ".bin";
    if (sciezka.isEmpty()) return false;

    QFile plik(sciezka);
    // Próba otwarcia pliku w trybie zapisu
    if (!plik.open(QIODevice::WriteOnly)) {
        return false;
    }

    // Użycie QDataStream do binarnej serializacji obiektu
    QDataStream out(&plik);
    out << *this; // Wymaga przeciążonego operatora << dla klasy Historia_egzaminow

    plik.close();
    return true;
}

/**
 * Wczytuje historię egzaminów z podanego pliku binarnego.
 */
bool Historia_egzaminow::wczytaj_instancje(const QString &sciezka)
{
    if (sciezka.isEmpty()) return false;

    QFile plik(sciezka);
    // Próba otwarcia pliku w trybie tylko do odczytu
    if (!plik.open(QIODevice::ReadOnly)) {
        qCritical() << "Nie można otworzyć pliku do odczytu:" << plik.errorString();
        return false;
    }

    // Deserializacja danych z pliku prosto do pól klasy
    QDataStream in(&plik);
    in >> *this; // Wymaga przeciążonego operatora >> dla klasy Historia_egzaminow

    plik.close();
    qDebug() << "Pomyślnie wczytano instancję. Liczba sesji:" << lista_egzaminow.size();
    return true;
}

