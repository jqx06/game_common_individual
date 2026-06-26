#include "character.h"
#include "ui_character.h"
#include <QPixmap>
#include <QPalette>
#include "musicmanager.h"
#include <QMediaPlayer>
#include <QAudioOutput>

character::character(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::character)
    , selectedClass("")
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/character_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "character");
}

character::~character()
{
    delete ui;
}

void character::on_character_button1_clicked(bool checked)
{
    Q_UNUSED(checked);
    selectedClass = "战士";
    emit classSelected(selectedClass);
    this->hide();
}
void character::on_character_button2_clicked(bool checked)
{
    Q_UNUSED(checked);
    selectedClass = "法师";
    emit classSelected(selectedClass);
    this->hide();
}
void character::on_character_button3_clicked(bool checked)
{
    Q_UNUSED(checked);
    selectedClass = "富商";
    emit classSelected(selectedClass);
    this->hide();
}
void character::on_character_button4_clicked(bool checked)
{
    Q_UNUSED(checked);
    selectedClass = "盗贼";
    emit classSelected(selectedClass);
    this->hide();
}
void character::on_character_button5_clicked(bool checked)
{
    Q_UNUSED(checked);
    selectedClass = "刺客";
    emit classSelected(selectedClass);
    this->hide();
}
