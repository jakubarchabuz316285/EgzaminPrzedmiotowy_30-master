#ifndef URN_H
#define URN_H
#include <random>
#include <QVector>
#include <QQueue>

// Klasa do losowania - losuje tylko indkeksy (liczby całkowite),
// to program glowny interpretuje te indeksy jako numery pytan.

class Urn
{
    //Pola klucozwe:
    QVector<int> m_elementy;
    int m_zakresLos,m_liczbaElementow;
    std::mt19937 m_generator;

    //Pola dodatkowe (do ksztaltowania rozkładu):
    QVector<float> m_liczLos;
    QVector<float> m_rozklad;
    QVector<float> m_dystr;

    //Metody uzytkowe wewnętrzne.:
    void ustaw();
    void generujRozklad();
    void generujDytsr();
    void normalizujLiczebnosci();
    int losujZDystr();

public:
    //Konstruktory:
    Urn(int liczbaElementow);
    Urn();
    //Metody użytkowe zewnętrzne:
    QVector<int> losuj(int numPyt);
    QVector<int> losujRedukcjaPowt(int numPyt);
    bool maElementy(){return (m_elementy.size()>0);}
    void setLiczElem(int liczbaElementow);
};
#endif // URN_H
