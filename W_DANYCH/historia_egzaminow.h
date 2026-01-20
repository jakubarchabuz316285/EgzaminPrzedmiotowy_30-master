#ifndef HISTORIA_EGZAMINOW_H
#define HISTORIA_EGZAMINOW_H
#include "qdatetime.h"
#include "qdir.h"
#include "qglobal.h"
#include "qimage.h"
#include "pytanie.h"


class Historia_egzaminow
{
public:
    struct Dane_pytania
    {
        int numer = 0;
        int blok = 0;
        QString tresc;
    };

    struct Sesja_egzaminacyjna
    {
        QString przedmiot;
        QDateTime data;
        QVector<Dane_pytania> wylosowane;
        QVector<Dane_pytania> odrzucone;
    };

private:
    QVector<Sesja_egzaminacyjna> lista_egzaminow;
    QString sciezka_zapisu = "historia.bin";

public:
    Historia_egzaminow() = default;
    ~Historia_egzaminow() = default;

    void dodaj_egzamin(const QString &przedmiot, const QVector<Pytanie> &wylosowane_p, const QVector<Pytanie> &odrzucone_p);
    const QVector<Sesja_egzaminacyjna> &get_historia() const;
    void wyczysc();
    void set_sciezka(const QString &sciezka);
    bool zapisz_instancje() const;
    bool wczytaj_instancje(const QString &sciezka);

    friend QDataStream &operator<<(QDataStream &out, const Dane_pytania &d) {
        out << d.numer << d.blok << d.tresc;
        return out;
    }

    friend QDataStream &operator<<(QDataStream &out, const Sesja_egzaminacyjna &s)
    {
        out << s.przedmiot
            << s.data
            << s.wylosowane
            << s.odrzucone;
        return out;
    }

    friend QDataStream &operator<<(QDataStream &out, const Historia_egzaminow &h)
    {
        out << h.lista_egzaminow;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Dane_pytania &d) {
        in >> d.numer >> d.blok >> d.tresc;
        return in;
    }

    friend QDataStream &operator>>(QDataStream &in, Sesja_egzaminacyjna &s)
    {
        in >> s.przedmiot >> s.data >> s.wylosowane >> s.odrzucone;
        return in;
    }

    friend QDataStream &operator>>(QDataStream &in, Historia_egzaminow &h)
    {
        in >> h.lista_egzaminow;
        return in;
    }
};

#endif // HISTORIA_EGZAMINOW_H
