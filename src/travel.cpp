#include "travel.h"
#include "ui_travel.h"

travel::travel(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::travel)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);
}

travel::~travel()
{
    delete ui;
}

void travel::selectAndClose(const QString& sceneName)
{
    emit destinationSelected(sceneName);
    accept();
}

void travel::on_button_village_clicked()
{
    selectAndClose("village");
}

void travel::on_button_forest_clicked()
{
    selectAndClose("forest");
}

void travel::on_button_cave_clicked()
{
    selectAndClose("cave");
}

void travel::on_button_town_clicked()
{
    selectAndClose("town");
}

void travel::on_button_city_clicked()
{
    selectAndClose("city");
}

void travel::on_button_cancel_clicked()
{
    reject();
}
