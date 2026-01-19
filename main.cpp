#define MAIN
#include "W_PREZENTACJI\mainwindow.h"

#ifdef MAIN
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#endif

#ifdef SEMIN

#include "urn.h"
#include <QVector>
#include <QDebug>
#include <QQueue>
#include <algorithm>

const int NUM_PYT = 12;
const int NUM_LOS = 7;
const int NUM_ZEST = 30;

QVector<int> rest(QVector<int> to_remove, int pula)
{
    QVector<int> result(pula);
    std::iota(result.begin(), result.end(), 1);

    result.erase(
        std::remove_if(result.begin(), result.end(),
                        [&to_remove](int num)
                        {
                            return std::find(to_remove.begin(), to_remove.end(), num) != to_remove.end();
                        }
                ), result.end());
    return result;
}



int main(int argc, char *argv[])
{
    Urn urna(NUM_PYT);
    //tworzenie nowego zestatwu:
    for(int zest = 0; zest <NUM_ZEST;zest++)
    {
        auto zestaw = urna.losujRedukcjaPowt(NUM_LOS);
        std::transform(zestaw.begin(),zestaw.end(),zestaw.begin(),[](int x){return x+1;});
        auto pdst = zestaw.first(3);
        std::sort(pdst.begin(),pdst.end());
        auto alt1 = zestaw.last(4);
        std::sort(alt1.begin(),alt1.end());
        auto alt2 = rest(zestaw,NUM_PYT);
        qInfo() << zest+1 << ": " << pdst  << alt1 << alt2;
    }
}

#endif

#ifdef DEBUG

#include "urn.h"
#include <QVector>
#include <QDebug>
#include <QQueue>

const int NUM_PYT = 20;
const int NUM_LOS = 3;
const int NUM_REP = 4;
const int NUM_TEST = 10000;

int countRep(QQueue<QVector<int>> &que)
{
    int wyn = 0;
    const int SIZE =  NUM_REP*NUM_LOS;
    std::array<int,SIZE> linData;
    int idx=0;
    for(int vec=0;vec<NUM_REP;vec++)
        for(int elem = 0; elem < NUM_LOS; elem++)
            linData[idx++] = que[vec][elem];
    for(int i = 0; i<SIZE-1;i++ )
        for(int j=i+1;j<SIZE;j++)
            if(linData[i]==linData[j])
                wyn++;
    return wyn;
}

int main(int argc, char *argv[])
{
    Urn ref(NUM_PYT);
    Urn testNC(NUM_PYT);

    QQueue<QVector<int>> refQ;
    QQueue<QVector<int>> testNCQ;
    for(int t = 0;t<NUM_REP-1;t++)
    {
        refQ.push_front(ref.losuj(NUM_LOS));
        testNCQ.push_front(testNC.losujRedukcjaPowt(NUM_LOS));
    }
    std::array<int,2> wyniki = {};
    for(int t = 0;t<NUM_TEST-NUM_REP+1;t++)
    {
        //push:
        refQ.push_front(ref.losuj(NUM_LOS));
        testNCQ.push_front(testNC.losujRedukcjaPowt(NUM_LOS));
        //eval:

        wyniki[0] += countRep(refQ);
        wyniki[1] += countRep(testNCQ);

        //pop front:
        refQ.pop_back();
        testNCQ.pop_back();
    }
    qInfo() << "REFERENCYJNY:" << (double)wyniki[0]/NUM_TEST;
    qInfo() << "NORM CIAGLA :" << (double)wyniki[1]/NUM_TEST;

    return 0;
}



#endif


#ifdef DEBUG_POWT

#include "urn.h"
#include <QVector>
#include <QDebug>
#include <deque>

const int NUM_PYT = 20;
const int NUM_LOS = 3;
const int NUM_REP = 3;
const int NUM_TEST = 10000;
const int NUM_PTS  = 10;


int countRep(std::deque<QVector<int>> &que)
{
    int wyn = 0;
    const int SIZE =  NUM_REP*NUM_LOS;
    std::array<int,SIZE> linData;
    int idx=0;
    for(int vec=0;vec<NUM_REP;vec++)
        for(int elem = 0; elem < NUM_LOS; elem++)
            linData[idx++] = que[vec][elem];
    for(int i = 0; i<SIZE-1;i++ )
        for(int j=i+1;j<SIZE;j++)
            if(linData[i]==linData[j])
                wyn++;
    return wyn;
}
int eksperymnet(int inc)
{
    qInfo() << "Tworzenie:";
    Urn testD2(NUM_PYT,TypNorm::normDziel2);
    Urn::setPrzyrostLiczebnosci(inc);
    std::deque<QVector<int>> testD2Q;
    qInfo() << "Rozbieg:";
    testD2Q.push_front(testD2.losujRedukcjaPowt(NUM_LOS));
    testD2Q.push_front(testD2.losujRedukcjaPowt(NUM_LOS));
    int wynik = 0;
    qInfo() << "TESTY:";
    for(int t = 0;t<NUM_TEST-2;t++)
    {
        testD2Q.push_front(testD2.losujRedukcjaPowt(NUM_LOS));
        wynik += countRep(testD2Q);
        testD2Q.pop_back();
    }
    return wynik;
}

int model(double prob)
{
    return round(pow(1.5896*prob,-2.67));
}

int main(int argc, char *argv[])
{
    std::array<double,NUM_PTS> wyniki = {};
    int increment = 1;
    for(int punkt = 0; punkt< NUM_PTS; punkt++)
    {
        wyniki[punkt] = (double)eksperymnet(increment)/NUM_TEST;

        qInfo() << "INC :" << increment << " : " << wyniki[punkt] << ": " << model(wyniki[punkt]);
        increment*=2;
    }

    return 0;
}



#endif
