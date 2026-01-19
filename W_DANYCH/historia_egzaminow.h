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

    void dodaj_egzamin(const QString &przedmiot, const QVector<Pytanie> &wylosowane_p, const QVector<Pytanie> &odrzucone_p)
    {
        Sesja_egzaminacyjna sesja;
        sesja.przedmiot = przedmiot;
        sesja.data = QDateTime::currentDateTime();

        for (const Pytanie &p : wylosowane_p)
            sesja.wylosowane.append({p.getNumer(), p.getBlok()});

        for (const Pytanie &p : odrzucone_p)
            sesja.odrzucone.append({p.getNumer(), p.getBlok()});

        lista_egzaminow.append(sesja);
    }

    const QVector<Sesja_egzaminacyjna> &get_historia() const
    {
        return lista_egzaminow;
    }

    void wyczysc()
    {
        lista_egzaminow.clear();
    }

    void set_sciezka(const QString &sciezka)
    {
        sciezka_zapisu = sciezka;
    }

    bool zapisz_instancje() const
    {
        QString sciezka = QDateTime::currentDateTime().toString("raport_yyyyMMdd_hhmmss") + ".bin";
        if (sciezka.isEmpty()) return false;

        QFile plik(sciezka);
        if (!plik.open(QIODevice::WriteOnly)) {
            return false;
        }

        QDataStream out(&plik);
        out << *this;

        plik.close();
        return true;
    }
    bool wczytaj_instancje(const QString &sciezka)
    {
        if (sciezka.isEmpty()) return false;

        QFile plik(sciezka);
        if (!plik.open(QIODevice::ReadOnly)) {
            qCritical() << "Nie można otworzyć pliku do odczytu:" << plik.errorString();
            return false;
        }

        QDataStream in(&plik);
        in >> *this;

        plik.close();
        qDebug() << "Pomyślnie wczytano instancję. Liczba sesji:" << lista_egzaminow.size();
        return true;
    }
    friend QDataStream &operator<<(QDataStream &out, const Dane_pytania &d)
    {
        out << d.numer << d.blok;
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

    friend QDataStream &operator>>(QDataStream &in, Dane_pytania &d)
    {
        in >> d.numer >> d.blok;
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
