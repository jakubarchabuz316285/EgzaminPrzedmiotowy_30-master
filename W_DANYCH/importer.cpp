#include "importer.h"
#include <QDir>
#include <QDebug>

// KONSTRUKTORY:

Importer::Importer(QObject *parent, QString dir)
    : QObject{parent}, m_dir(dir)
{
    wykryjPrzedmioty();
    m_aktPrzedmiot = m_listaPrzedmiotow.first();
}

// JEDYNA WSPÓLNA, ZŁOŻONA METODA DLA WSZYSTKICH IMPORTERÓW:

// Metoda ustala nazwy obsługiwanych przedmiotów na podstawie struktury
// katalogów w zadanym katalogu bazy pytań (katalog ten podaje się podczas
// tworzenia instancji importera).
void Importer::wykryjPrzedmioty()
{
    QDir pytania = QDir::current();
    pytania.cd(m_dir);
    pytania.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    m_listaPrzedmiotow = pytania.entryList();
}

