#include "historia_egzaminow.h"

void Historia_egzaminow::dodaj_egzamin(const QString &przedmiot, const QVector<Pytanie> &wylosowane_p, const QVector<Pytanie> &odrzucone_p)
{
    Sesja_egzaminacyjna sesja;
    sesja.przedmiot = przedmiot;
    sesja.data = QDateTime::currentDateTime();

    for (const Pytanie &p : wylosowane_p)
        sesja.wylosowane.append({p.getNumer(), p.getBlok(), p.getCzystyNaglowek()});

    for (const Pytanie &p : odrzucone_p)
        sesja.odrzucone.append({p.getNumer(), p.getBlok(), p.getCzystyNaglowek()});

    lista_egzaminow.append(sesja);
}

const QVector<Historia_egzaminow::Sesja_egzaminacyjna> &Historia_egzaminow::get_historia() const
{
    return lista_egzaminow;
}

void Historia_egzaminow::wyczysc()
{
    lista_egzaminow.clear();
}

void Historia_egzaminow::set_sciezka(const QString &sciezka)
{
    sciezka_zapisu = sciezka;
}

bool Historia_egzaminow::zapisz_instancje() const
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
bool Historia_egzaminow::wczytaj_instancje(const QString &sciezka)
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


