#ifndef IMPORTERBLOKOWY_H
#define IMPORTERBLOKOWY_H

#include "importer.h"

// Klasa importer blokowy jest już konkretnym importerem czytajacym cały blok pytań
// z pojdeynczego pliku o nazwie blokX (X to duza litera od A do Z). Litera A to blok 0

class ImporterBlokowy : public Importer
{
    Q_OBJECT
    void wczytajBlok(QString &path, QVector<Pytanie> &dst, int numBlok);
public:
    explicit ImporterBlokowy(QObject *parent, QString dir);
    void wczytajDane() override;
};

#endif // IMPORTERBLOKOWY_H
