#ifndef CHARACTER_H
#define CHARACTER_H

#include <QDialog>
#include <QString>

namespace Ui {
class character;
}

class character : public QDialog
{
    Q_OBJECT

public:
    explicit character(QWidget *parent = nullptr);
    ~character();

signals:
    void Return();
    void classSelected(QString className);

private slots:
    void on_character_button1_clicked(bool checked);
    void on_character_button2_clicked(bool checked);
    void on_character_button3_clicked(bool ckecked);
    void on_character_button4_clicked(bool checked);
    void on_character_button5_clicked(bool checked);

private:
    Ui::character *ui;
    QString selectedClass;
};

#endif // CHARACTER_H
