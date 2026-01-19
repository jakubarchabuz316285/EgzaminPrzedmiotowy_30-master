#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>
#include <QVector>
#include "pytanie.h"

// Importer jest klasą bazową definiumącą podstawowe narzędzia potrzebne we wszytkich
// mozliwych impoertreach (klasy pochodne od tej) jakie mogą współpracować z programem.
// Pozwala to na stsounkowo latwą podmianę algorytmu wczytywania pytań bez potrzeby
// ingerenecji w resztę programu (interfejs jest zdefiniowany w tej klasie).

class Importer : public QObject
{
    Q_OBJECT
protected:
    int m_numBlok = 0;
    QString m_dir;
    std::vector<QVector<Pytanie>> m_bloki;
    QStringList m_listaPrzedmiotow;
    QString m_aktPrzedmiot;
    void wykryjPrzedmioty();
public:
    virtual void wczytajDane() = 0;

    explicit Importer(QObject *parent, QString dir);
    QStringList getListaPrzedmiotow() {return m_listaPrzedmiotow;}
    QVector<Pytanie> & getBlok(int num) {return m_bloki[num];}
    void setPrzedmiot(QString nazwa) {m_aktPrzedmiot = nazwa;}
    QString getPrzedmiot() {return m_aktPrzedmiot;}
    int getNumBlok() {return m_numBlok;}

signals:
};

#endif // IMPORTER_H
