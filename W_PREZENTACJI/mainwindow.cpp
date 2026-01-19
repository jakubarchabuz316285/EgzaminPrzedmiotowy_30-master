#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QHBoxLayout>

// KONSTRUKTORY, SETUP I DESTRUKTOR:

                                   MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); // Wbudowane w Qt

    // Tablice wskaźników do sterowania zbiorowego. Pozwalają odwoływać się do kontrolek po numerze,
    // co ułatwia proceduralną modyfikację interfejsu (np. w pętli) i realizację zasady DRY:
    m_pytDisp      = {ui->pytA, ui->pytB};
    m_pytDispPelny = {ui->pytAPelny, ui->pytBPelny};
    m_pytLicz      = {ui->liczA, ui->liczB};
    m_genPush      = {ui->pushGenBA, ui->pushGenBB};
    m_wybPush      = {ui->pushWybBA, ui->pushWybBB};
    m_tabs         = {ui->tabA, ui->tabB};

    // Ustawienie comboboxa z nazwami przedmiotów:
    ui->przedmiot->clear();
    ui->przedmiot->addItems(m_uslugi.getListaPrzedmiotow());
    ui->przedmiot->setCurrentIndex(1); // Domyślnie PPK

    // Moja inicjalizacja stanu początkowego kontrolek w GUI:
    ustawGUI();

    // Podłączenie sygnałów z warstwy usług:
    connect(&m_uslugi, SIGNAL(wypisz(QVector<Pytanie>&, int)),
            this, SLOT(wypisz(QVector<Pytanie>&, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ustawGUI()
{
    wylaczWszystko();
    on_pushWyczysc_clicked();
    ui->pushWczytaj->setEnabled(true);
    QString aktPrzedmiot = m_uslugi.getAktPrzedmiot();
    this->setWindowTitle(aktPrzedmiot);
    ui->tabBloki->setCurrentIndex(0);
}


// METODY (AKCJE):

// Metoda do zbiorowej dezaktywacji większości kontrolek:
void MainWindow::wylaczWszystko()
{
    for(int blok = 0; blok < MAX_LICZ_BLOKOW; blok++)
    {
        m_pytDisp[blok]->setEnabled(false);
        m_pytDispPelny[blok]->setEnabled(false);
        m_pytLicz[blok]->setEnabled(false);
        m_genPush[blok]->setEnabled(false);
        m_tabs[blok]->setEnabled(false);
    }
}

// Metoda wypisująca pełne wersje pytań.
// Program zawiera dwa widgety w tej samej lokalizacji geometrycznej służące do wypisywania pytań:
// 1. QTextBrowser – umożliwia wyświetlanie pełnych pytań w formacie HTML,
// 2. QTableWidget – umożliwia prezentację pytań w formie tabeli z możliwością dodania checkboxów.
// Widgety te są umieszczone w tych samych miejscach na zakładkach blokowych,
// a widoczny jest zawsze tylko jeden z nich – zależnie od trybu wyświetlania pytań.
// Poniższa metoda wyświetla tylko QTextBrowser (ukrywając tabelę), ponieważ służy do prezentacji
// pełnych wersji wybranych pytań:
void MainWindow::wypiszPelnePytania(QVector<Pytanie> &pytania, int blok)
{
    // Przełączenie widoczności na QTextBrowser:
    m_pytDisp[blok]->hide();
    m_pytDispPelny[blok]->show();
    m_pytDispPelny[blok]->setEnabled(true);

    // Kompozycja treści do wyświetlenia – dwa warianty:
    // 1. Wyświetlenie pytań z rozbiciem (gdy zaznaczony checkDekomponuj),
    // 2. Wyświetlenie pełnych pytań tematycznie (gdy checkDekomponuj jest odznaczony).
    //
    // Tryb standardowy (checkDekomponuj wyłączony):
    //   Czerwone 1 → Zielone 1 → Niebieskie 1 → Czerwone 2 → ...
    //
    // Tryb z rozbiciem (checkDekomponuj włączony):
    //   Czerwone 1 → Czerwone 2 → Czerwone 3 → Zielone 1 → ...
    //
    // Tryb z rozbiciem przyspiesza ocenianie – najpierw pytania kluczowe (np. „czerwone”),
    // a dopiero potem pytania podnoszące ocenę. Tryb standardowy jest bardziej naturalny
    // dydaktycznie, ale może wydłużyć egzamin.
    QString tresc = "";
    if(ui->checkDekomponuj->isChecked()) // Tryb z rozbiciem
    {
        for(int pyt = 0; pyt < pytania.size(); pyt++)
            tresc += pytania[pyt].getNaglowek();
        for(int pyt = 0; pyt < pytania.size(); pyt++)
            tresc += pytania[pyt].getWiedza();
        for(int pyt = 0; pyt < pytania.size(); pyt++)
            tresc += pytania[pyt].getZrozumienie();
        for(int pyt = 0; pyt < pytania.size(); pyt++)
            tresc += pytania[pyt].getDyskusja();
    }
    else // Tryb standardowy
    {
        for(int pyt = 0; pyt < pytania.size(); pyt++)
            tresc += pytania[pyt].getTresc();
    }

    // Wyświetlenie treści na ekranie:
    m_pytDispPelny[blok]->setText(tresc);
    ui->checkDekomponuj->setEnabled(true);
}

// Metoda przygotowująca tabelę pytań, gdy jest ich więcej niż 3:
void MainWindow::tworzTabele(int blok, int num)
{
    // Parametryzacja wyglądu tabeli:
    m_pytDisp[blok]->clear();
    m_pytDisp[blok]->setEnabled(true);
    m_pytDisp[blok]->setColumnCount(2);
    m_pytDisp[blok]->setRowCount(num);
    m_pytDisp[blok]->setColumnWidth(0, 50);
    m_pytDisp[blok]->setColumnWidth(1, 990);
    m_pytDisp[blok]->setHorizontalHeaderItem(0, new QTableWidgetItem("Usuń"));
    m_pytDisp[blok]->setHorizontalHeaderItem(1, new QTableWidgetItem("Pytanie"));

    // Wstawienie checkboxów do kolumny 0:
    // Ważna uwaga – po dodaniu checkboxa do tabeli, trudno później odczytać jego stan bezpośrednio.
    // Dlatego jego wskaźnik zapisywany jest do m_chkBoxes, aby później łatwo sprawdzać zaznaczenie.
    // Dzięki relacji rodzic–dziecko, tabela zarządza cyklem życia checkboxów i usuwa je automatycznie.
    for(int pyt = 0; pyt < num; pyt++)
    {
        // Aby checkbox był estetycznie wycentrowany w komórce tabeli,
        // należy użyć QWidgeta z poziomym layoutem i osadzić w nim checkbox.
        QWidget *pWidget = new QWidget();
        QCheckBox *pCheckBox = new QCheckBox();
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(pCheckBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);

        // Dodanie złożonego widgetu do komórki tabeli i zapisanie wskaźnika:
        m_pytDisp[blok]->setCellWidget(pyt, 0, pWidget);
        m_chkBoxes[blok].push_back(pCheckBox);
    }
}

// Metoda sprawdzająca, które pytania zostały oznaczone do usunięcia:
QVector<int> MainWindow::sprawdzDoUsuniecia(int blok)
{
    int numRows = m_pytDisp[blok]->rowCount();
    QVector<int> pytNums;
    for(int row = 0; row < numRows; row++)
    {
        if(m_chkBoxes[blok][row]->isChecked())
            pytNums.push_back(row);
    }
    return pytNums;
}

// Metoda dezaktywująca wszystkie checkboxy w danym bloku:
void MainWindow::wylaczCheckBokxy(int blok)
{
    int numRows = m_pytDisp[blok]->rowCount();
    for(int row = 0; row < numRows; row++)
        m_chkBoxes[blok][row]->setEnabled(false);
}

// Metoda usuwająca pytania oznaczone do usunięcia:
void MainWindow::wyborPytan(int blok)
{
    // Sprawdzenie, które pytania zostały zaznaczone:
    QVector<int> numToRem = sprawdzDoUsuniecia(blok);

    // Sprawdzenie zgodności liczby zaznaczonych pytań z wartością w spinboxie.
    // W razie niezgodności – komunikat i przerwanie działania.
    if(numToRem.size() != m_pytLicz[blok]->value())
    {
        ui->statusBar->showMessage("Niewłaściwa liczba pytań!");
        return;
    }

    // Jeśli liczba się zgadza – wyłącz checkboxy:
    wylaczCheckBokxy(blok);

    // Wywołanie funkcji usługowej do oczyszczenia listy pytań:
    m_uslugi.odznaczPytania(numToRem, blok);

    // Dezaktywacja przycisku wyboru:
    m_wybPush[blok]->setEnabled(false);
}

// Metoda wywołująca generowanie pytań dla danego bloku.
// Wydzielona w celu uniknięcia powielania kodu:
void MainWindow::genrujPytania(int blok)
{
    // Dezaktywacja kontrolek GUI:
    m_genPush[blok]->setEnabled(false);
    m_pytLicz[blok]->setEnabled(false);
    m_wybPush[blok]->setEnabled(false);
    ui->pushWyczysc->setEnabled(true);

    // Wywołanie usługi generowania pytań – przekazywana jest liczba *dodatkowych* pytań,
    // ponieważ domyślnie generowane są 3 pytania bazowe.
    m_uslugi.losujPytania(blok, m_pytLicz[blok]->value());
}

// SLOTY (REAKCJE):

// Ta metoda jest slotem reagującym na sygnał „wypisz” z warstwy usług (program.cpp)
void MainWindow::wypisz(QVector<Pytanie>& pytania, int blok)
{
    if(pytania.size() == MIN_LICZ_PYT) // Jeśli liczba wylosowanych pytań to 3 (wartość domyślna)
    {
        // Wypisanie pytań w QTextBrowser w pełnej wersji i formacie HTML
        wypiszPelnePytania(pytania, blok);
    }
    else // Jeśli jest więcej pytań:
    {
        // Przełączenie wyświetlania na QTableWidget:
        m_pytDisp[blok]->show();
        m_pytDispPelny[blok]->hide();

        // Ustalenie liczby pytań do wygenerowania tabeli:
        int numPyt = pytania.size();

        // Wygenerowanie tabeli i wypełnienie jej tylko nagłówkami pytań:
        tworzTabele(blok, numPyt);
        for(int pyt = 0; pyt < numPyt; pyt++)
            m_pytDisp[blok]->setItem(pyt, 1, new QTableWidgetItem(pytania[pyt].getCzystyNaglowek()));

        // Aktywacja przycisku zatwierdzającego wybór:
        m_wybPush[blok]->setEnabled(true);

        // Dezaktywacja checkboxa do dekompozycji pytań – istotne, ponieważ
        // zmiana jego stanu powoduje aktywną zmianę sposobu wyświetlania
        // treści pytań. Nie powinno być to możliwe, dopóki wybór pytań do wypisania
        // nie zostanie zakończony:
        ui->checkDekomponuj->setEnabled(false);
    }
}

// Metoda dokonująca resetu stanu kontrolek i widgetów tekstowych po zakończeniu egzaminu:
void MainWindow::on_pushWyczysc_clicked()
{
    for(int blok = 0; blok < m_uslugi.getLiczbaBlokow(); blok++)
    {
        m_tabs[blok]->setEnabled(true);
        m_pytLicz[blok]->setEnabled(true);
        m_genPush[blok]->setEnabled(true);
        m_pytDisp[blok]->clear();
        m_pytDispPelny[blok]->clear();
        m_chkBoxes[blok].clear();
        m_pytLicz[blok]->setValue(0);
    }
    ui->tabBloki->setCurrentIndex(0);
}

// Cztery kolejne metody są reakcjami na naciśnięcie konkretnych przycisków.
// Aby nie powtarzać kodu (zasada DRY), delegują one zadania do metod prywatnych,
// wskazując, którego bloku dotyczy reakcja:

void MainWindow::on_pushGenBA_clicked()
{
    genrujPytania(0); // przekazuje numer bloku
}

void MainWindow::on_pushGenBB_clicked()
{
    genrujPytania(1); // przekazuje numer bloku
}

void MainWindow::on_pushWybBA_clicked()
{
    wyborPytan(0); // przekazuje numer bloku
}

void MainWindow::on_pushWybBB_clicked()
{
    wyborPytan(1); // przekazuje numer bloku
}

// Metoda reakcji na zmianę przedmiotu – wywołuje usługę zmiany przedmiotu w warstwie usług
// (czyści całą bazę pytań). Dodatkowo przywraca GUI do stanu początkowego.
void MainWindow::on_przedmiot_currentTextChanged(const QString &i_przedmiot)
{
    m_uslugi.setPrzedmiot(i_przedmiot);
    ustawGUI();
}

// Metoda reakcji na wczytanie pytań. Na czas wczytywania dezaktywuje wszystkie
// kontrolki, które mogłyby wprowadzić program w nieprawidłowy stan.
// Następnie wywołuje usługę wczytania bazy pytań z plików na dysku (dotychczasowa
// baza zostaje całkowicie wymieniona). Potem przywraca GUI do stanu gotowości do generowania pytań.
void MainWindow::on_pushWczytaj_clicked()
{
    wylaczWszystko();
    m_uslugi.wczytajPytania();
    ui->pushWczytaj->setEnabled(false);
    on_pushWyczysc_clicked();
}

// Metoda reakcji na zmianę sposobu wypisywania pytań. Sam stan checkboxa jest
// sprawdzany przy wypisywaniu, ale ta reakcja dodatkowo wywołuje usługę
// ponownego wypisania pytań (które powinny być już na ekranie).
// Odwołujemy się do warstwy usług, ponieważ warstwa prezentacji (GUI) nie
// przechowuje trwale wylosowanych pytań – są one przekazywane tymczasowo.
void MainWindow::on_checkDekomponuj_stateChanged(int state)
{
    Q_UNUSED(state);
    m_uslugi.ponownieWypiszWylosowane(ui->tabBloki->currentIndex());
}
