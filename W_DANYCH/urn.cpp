#include "urn.h"
#include <QDebug>

// KONSTRUKTORY i INICJALIZACJA:

Urn::Urn()
{
    setLiczElem(10);
    ustaw();
}

Urn::Urn(int liczbaElementow)
{
    setLiczElem(liczbaElementow);
    ustaw();
}

void Urn::ustaw()
{
    std::random_device rseed;
    m_generator.seed(rseed());
    m_zakresLos = m_liczbaElementow - 1;
}

// KLUCZOWE USŁUGI ZEWNĘTRZNE:

QVector<int> Urn::losuj(int numPyt)
// Standardowe losowanie podanej liczby indeksów z pełnej puli, bez powtórzeń
{
    QVector<int> numery(numPyt);
    // Losowanie bez zwracania (tylko w ramach pojedynczego wywołania metody):
    for(int i = 0; i < numPyt; i++)
    {
        std::uniform_int_distribution<int> zakres(0, m_zakresLos);
        int numer = zakres(m_generator);
        numery[i] = m_elementy[numer];
        std::swap(m_elementy[numer], m_elementy[m_zakresLos--]);
    }
    // Reset zakresu losowania - przywrócenie wszystkich indeksów do puli.
    m_zakresLos = m_liczbaElementow - 1;
    return numery;
}

QVector<int> Urn::losujRedukcjaPowt(int numPyt)
// Zaawansowane losowanie podanej liczby pytań z puli, bez powtórzeń.
// Losowanie to modyfikuje rozkład prawdopodobieństwa tak, aby wylosowane pytania
// miały zmniejszone prawdopodobieństwo trafienia w kolejnych losowaniach.
{
    // Obowiązkowa normalizacja liczebności, aby suma prawdopodobieństw była równa liczbie elementów w puli.
    // Jest to konieczne, ponieważ po losowaniu liczebności wartości mogą się zwiększać,
    // co powodowałoby przekroczenie sumy prawdopodobieństw powyżej 1.0.
    normalizujLiczebnosci();

    QVector<int> numery(numPyt);

    // Pętla wykonuje losowanie bez zwracania zadanej liczby pytań:
    for(int idx = 0; idx < numPyt; idx++)
    {
        // W każdej iteracji generowany jest nowy rozkład prawdopodobieństwa,
        // ponieważ zbiór modyfikuje się wskutek losowania bez zwracania.
        // Jest to najprostsze rozwiązanie, choć nie optymalne pod względem liczby operacji.
        // W tym programie takie losowanie trwa ułamek sekundy i jest wykonywane rzadko,
        // więc prostota rozwiązania jest ważniejsza niż maksymalna wydajność.
        generujRozklad();  // Buduje rozkład prawdopodobieństwa na podstawie znormalizowanych liczebności
        generujDytsr();    // Buduje dystrybuantę na podstawie rozkładu

        // Algorytm losowania z dystrybuanty pozwalający wybrać element z nierównomiernym rozkładem:
        int numer = losujZDystr();

        Q_ASSERT_X(0 <= numer && numer <= m_zakresLos, "Urna",
                   (QString("Z dystrybuanty wylosowano nieprawidłowy indeks: ") + QString::number(numer)).toLatin1().data());

        numery[idx] = m_elementy[numer];

        // Modyfikacja liczebności: mnożenie przez 2 powoduje, że prawdopodobieństwo ponownego
        // wylosowania tego indeksu jest dwukrotnie mniejsze niż obecnie.
        // Dzięki normalizacji pozostałe elementy mają zmienione prawdopodobieństwa.
        m_liczLos[numer] *= 2;

        // Usunięcie wylosowanego indeksu z puli na czas jednego wywołania metody:
        std::swap(m_elementy[numer], m_elementy[m_zakresLos]);
        std::swap(m_liczLos[numer], m_liczLos[m_zakresLos]);
        m_zakresLos--;
    }
    // Reset zakresu losowania - przywrócenie wszystkich indeksów do puli.
    m_zakresLos = m_liczbaElementow - 1;
    return numery;
}

void Urn::setLiczElem(int liczbaElementow)
{
    if(liczbaElementow != m_liczbaElementow)
    {
        // Wektory wymagające zmiany rozmiaru i reinicjalizacji:
        m_elementy.clear();
        m_elementy.resize(liczbaElementow);
        m_liczLos.clear();
        m_liczLos.resize(liczbaElementow);

        // Wektory wymagające jedynie zmiany rozmiaru (nadpisywane podczas użycia):
        m_rozklad.resize(liczbaElementow);
        m_dystr.resize(liczbaElementow);

        // Reset zawartości wektorów:
        for(int idx = 0; idx < liczbaElementow; idx++)
        {
            m_elementy[idx] = idx;
            m_liczLos[idx] = 1.0f;
        }

        // Aktualizacja pól sterujących losowaniem:
        m_liczbaElementow = liczbaElementow;
        m_zakresLos = m_liczbaElementow - 1;
    }
}

// ALGORYTMY WEWNĘTRZNE DO LOSOWANIA ZAAWANSOWANEGO:

void Urn::generujRozklad()
{
    // UWAGA: liczenie rozkładu odbywa się zawsze w aktualnym zakresie losowania
    // (po ewentualnym wykluczeniu wylosowanych indeksów w wyniku losowania bez zwracania).

    // Reset rozkładu prawdopodobieństwa:
    m_rozklad.fill(0.0f);
    int n = m_zakresLos + 1;

    // Obliczenie odwrotności liczebności (używając programowania funkcyjnego):
    std::transform(m_liczLos.begin(), m_liczLos.begin() + n,
                   m_rozklad.begin(), [](float arg) { return 1.0f / arg; });

    // Normalizacja rozkładu liczebności. Jest niezbędna, ponieważ przed losowaniem
    // suma prawdopodobieństw powinna być równa 1.0.
    // Po eliminacji indeksów i modyfikacji liczebności suma ta się zmienia,
    // więc należy ją ponownie wyznaczyć i poprawić rozkład.

    // Sumowanie prawdopodobieństw (suma powinna być różna od 1.0):
    float suma = std::accumulate(m_rozklad.begin(), m_rozklad.begin() + n, 0.0f);

    // Normalizacja - dzielenie każdego elementu przez sumę, aby uzyskać poprawny rozkład:
    std::transform(m_rozklad.begin(), m_rozklad.begin() + n,
                   m_rozklad.begin(), [suma](float arg) { return arg / suma; });
}

void Urn::generujDytsr()
{
    // Reset dystrybuanty:
    m_dystr.fill(0.0f);
    int n = m_zakresLos + 1;

    // Wyznaczenie dystrybuanty, czyli skumulowanego rozkładu prawdopodobieństwa.
    // Używamy tutaj mutowalnej lambdy, która sumuje bieżące wartości rozkładu.
    // Ostatni element powinien być równy 1.0.
    std::transform(m_rozklad.begin(), m_rozklad.begin() + n, m_dystr.begin(),
                   [sum = 0.0f](float arg) mutable { return sum += arg; });
}

void Urn::normalizujLiczebnosci()
{
    // Normalizacja liczebności po każdym losowaniu, aby dostosować
    // prawdopodobieństwa do sumy równej liczbie elementów w puli.
    // Zapobiega to nieskończonemu wzrostowi liczebności i stabilizuje działanie algorytmu.

    // Obliczenie sumy aktualnych liczebności:
    float suma = std::accumulate(m_liczLos.begin(), m_liczLos.end(), 0.0f);

    // Normalizacja liczebności tak, aby suma była równa liczbie elementów:
    std::transform(m_liczLos.begin(), m_liczLos.end(), m_liczLos.begin(),
                   [this, suma](float arg) { return arg * m_liczbaElementow / suma; });
}

int Urn::losujZDystr()
{
    // Wartości dystrybuanty nie są rozłożone równomiernie w przedziale <0.0, 1.0>,
    // co umożliwia losowanie elementów z różnym prawdopodobieństwem (patrz Algorytm Koła Fortuny).

    // Losowanie liczby rzeczywistej z przedziału <0.0, 1.0>:
    std::uniform_real_distribution<float> zakres(0.0f, 1.0f);
    float val = zakres(m_generator);

    // Znalezienie pierwszego elementu dystrybuanty większego lub równego wylosowanej wartości
    // i zwrócenie jego indeksu (jest to wylosowany indeks z puli):
    for(int idx = 0; idx <= m_zakresLos; idx++)
        if(val <= m_dystr[idx]) return idx;

    // Jeśli coś poszło nie tak (np. zaokrąglenie), zwracamy ujemny indeks:
    return -1;
}
