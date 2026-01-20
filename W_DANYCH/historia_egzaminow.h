#ifndef HISTORIA_EGZAMINOW_H
#define HISTORIA_EGZAMINOW_H

// Nagłówki Qt potrzebne do obsługi daty, plików, strumieni i kontenerów
#include "qdatetime.h"
#include "qdir.h"
#include "qglobal.h"
#include "qimage.h"

// Klasa Pytanie – wykorzystywana do tworzenia historii egzaminów
#include "pytanie.h"

// Klasa odpowiedzialna za przechowywanie i obsługę historii egzaminów
class Historia_egzaminow
{
public:
    // Struktura przechowująca podstawowe dane jednego pytania
    struct Dane_pytania
    {
        int numer = 0;      // Numer pytania
        int blok = 0;       // Numer bloku / działu
        QString tresc;      // Treść pytania
    };

    // Struktura opisująca jedną sesję egzaminacyjną
    struct Sesja_egzaminacyjna
    {
        QString przedmiot;                 // Nazwa przedmiotu
        QDateTime data;                    // Data i godzina egzaminu
        QVector<Dane_pytania> wylosowane;  // Lista wylosowanych pytań
        QVector<Dane_pytania> odrzucone;   // Lista odrzuconych pytań
    };

private:
    // Lista wszystkich zapisanych sesji egzaminacyjnych
    QVector<Sesja_egzaminacyjna> lista_egzaminow;

    // Domyślna ścieżka zapisu pliku binarnego z historią
    QString sciezka_zapisu = "historia.bin";

public:
    // Domyślny konstruktor
    Historia_egzaminow() = default;

    // Domyślny destruktor
    ~Historia_egzaminow() = default;

    // Dodaje nową sesję egzaminacyjną do historii
    void dodaj_egzamin(const QString &przedmiot,
                       const QVector<Pytanie> &wylosowane_p,
                       const QVector<Pytanie> &odrzucone_p);

    // Zwraca stałą referencję do historii egzaminów
    const QVector<Sesja_egzaminacyjna> &get_historia() const;

    // Czyści całą historię egzaminów
    void wyczysc();

    // Ustawia nową ścieżkę zapisu pliku
    void set_sciezka(const QString &sciezka);

    // Zapisuje obiekt historii do pliku binarnego
    bool zapisz_instancje(QString sciezka_p) const;

    // Wczytuje historię z podanego pliku binarnego
    bool wczytaj_instancje(const QString &sciezka);

    //Zapisuje odrzucone pytania z checkbox
    void dodaj_odrzucone_do_ostatniej_sesji(const QVector<Pytanie> &odrzucone_p);

    // Operator zapisu struktury Dane_pytania do QDataStream
    friend QDataStream &operator<<(QDataStream &out, const Dane_pytania &d)
    {
        out << d.numer << d.blok << d.tresc;
        return out;
    }

    // Operator zapisu struktury Sesja_egzaminacyjna do QDataStream
    friend QDataStream &operator<<(QDataStream &out, const Sesja_egzaminacyjna &s)
    {
        out << s.przedmiot
            << s.data
            << s.wylosowane
            << s.odrzucone;
        return out;
    }

    // Operator zapisu całej historii egzaminów do QDataStream
    friend QDataStream &operator<<(QDataStream &out, const Historia_egzaminow &h)
    {
        out << h.lista_egzaminow;
        return out;
    }

    // Operator odczytu struktury Dane_pytania z QDataStream
    friend QDataStream &operator>>(QDataStream &in, Dane_pytania &d)
    {
        in >> d.numer >> d.blok >> d.tresc;
        return in;
    }

    // Operator odczytu struktury Sesja_egzaminacyjna z QDataStream
    friend QDataStream &operator>>(QDataStream &in, Sesja_egzaminacyjna &s)
    {
        in >> s.przedmiot >> s.data >> s.wylosowane >> s.odrzucone;
        return in;
    }

    // Operator odczytu całej historii egzaminów z QDataStream
    friend QDataStream &operator>>(QDataStream &in, Historia_egzaminow &h)
    {
        in >> h.lista_egzaminow;
        return in;
    }
};

#endif // HISTORIA_EGZAMINOW_H
