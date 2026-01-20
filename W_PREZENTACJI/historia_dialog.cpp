#include "historia_dialog.h"
#include "ui_historia_dialog.h"
#include <QFileDialog>
historia_dialog::historia_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::historia_dialog)
{
    ui->setupUi(this);
}

historia_dialog::~historia_dialog()
{
    delete ui;
}

QString historia_dialog::get_sciezka_do_pliku()
{
    return sciezka_do_pliku;
}

void historia_dialog::ustawTekst(const QString &tekst) {
    ui->teskt_pytan->setHtml(tekst);
}

