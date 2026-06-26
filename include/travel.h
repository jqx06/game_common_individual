#ifndef TRAVEL_H
#define TRAVEL_H

#include <QDialog>

namespace Ui {
class travel;
}

class travel : public QDialog
{
    Q_OBJECT

public:
    explicit travel(QWidget *parent = nullptr);
    ~travel();

signals:
    void destinationSelected(const QString& sceneName);

private slots:
    void on_button_village_clicked();
    void on_button_forest_clicked();
    void on_button_cave_clicked();
    void on_button_town_clicked();
    void on_button_city_clicked();
    void on_button_cancel_clicked();

private:
    void selectAndClose(const QString& sceneName);

    Ui::travel *ui;
};

#endif // TRAVEL_H
