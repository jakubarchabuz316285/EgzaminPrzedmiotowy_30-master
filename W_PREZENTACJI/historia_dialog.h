#ifndef HISTORIA_DIALOG_H
#define HISTORIA_DIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
class historia_dialog;
}

class historia_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit historia_dialog(QWidget *parent = nullptr);
    ~historia_dialog();
    QString get_sciezka_do_pliku();
    void ustawTekst(const QString &tekst);

private slots:


private:
    Ui::historia_dialog *ui;
    QString sciezka_do_pliku ="";
};

#endif // HISTORIA_DIALOG_H
