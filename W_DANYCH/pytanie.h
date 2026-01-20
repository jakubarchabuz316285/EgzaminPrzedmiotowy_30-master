#ifndef PYTANIE_H
#define PYTANIE_H

#include <QString>

class Pytanie
{
    QString m_naglowek    = "";
    QString m_wiedza    = "";
    QString m_zrozum = "";
    QString m_dyskusja  = "";
    int m_numer;
    int m_blok;
public:
    Pytanie() = default;
    Pytanie(QString &rawData, int num, int blok);
    QString getTresc();
    QString getCzystyNaglowek() const;
    QString getNaglowek();
    QString getWiedza();
    QString getZrozumienie();
    QString getDyskusja();
    int getNumer ()const {return m_numer;}
    int getBlok ()const {return m_blok;}
};

#endif // PYTANIE_H
