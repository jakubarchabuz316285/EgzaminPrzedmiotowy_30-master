#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QTableWidget>
#include <QTextBrowser>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

#include "W_USLUG\program.h"
#include "W_DANYCH\pytanie.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushGenBA_clicked();
    void on_pushGenBB_clicked();
    void on_pushWyczysc_clicked();
    void on_przedmiot_currentTextChanged(const QString &spec);
    void on_pushWczytaj_clicked();
    void on_checkDekomponuj_stateChanged(int state);
    void on_pushWybBA_clicked();
    void on_pushWybBB_clicked();
    void wypisz(QVector<Pytanie>&,int);

private:
    static const int MAX_LICZ_BLOKOW = 2;
    static const int MIN_LICZ_PYT = 3;

    void ustawGUI();
    void wylaczWszystko();
    void tworzTabele(int,int);
    QVector<int> sprawdzDoUsuniecia(int);
    void wylaczCheckBokxy(int);
    void wyborPytan(int blok);
    void genrujPytania(int blok);
    void wypiszPelnePytania(QVector<Pytanie>&,int);

    Ui::MainWindow *ui;
    Program m_uslugi;

    std::array<QSpinBox*,MAX_LICZ_BLOKOW> m_pytLicz;
    std::array<QTextBrowser*,MAX_LICZ_BLOKOW> m_pytDispPelny;
    std::array<QTableWidget*,MAX_LICZ_BLOKOW> m_pytDisp;
    std::array<QPushButton*,MAX_LICZ_BLOKOW> m_genPush;
    std::array<QPushButton*,MAX_LICZ_BLOKOW> m_wybPush;
    std::array<QWidget*,MAX_LICZ_BLOKOW> m_tabs;
    std::array<std::vector<QCheckBox*>,2> m_chkBoxes;
};

#endif // MAINWINDOW_H
