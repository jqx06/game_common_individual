#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "function.h"

class Secondwindow;
class character;
class rule;
class village;
class forest;
class cave;
class town;
class city;
class SaveManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_button1_clicked(bool checked);
    void Onreturn();
    void on_create_character_clicked(bool checked);
    void on_rule_button_clicked(bool checked);
    void on_exit_button_clicked(bool checked);
    void on_memory_button_clicked(bool checked);
    void onClassSelected(QString className);
    void onGoToForest();
    void onNavigateTo(const QString& sceneName);

private:
    void startGameWithClass(QString className);
    void setupMainText();

    Ui::MainWindow *ui;
    character *characterwindow = nullptr;
    rule *rulewindow = nullptr;
    village *villagewindow = nullptr;
    forest *forestwindow = nullptr;
    cave *cavewindow = nullptr;
    town *townwindow = nullptr;
    city *citywindow = nullptr;
    ScreenControl *screenControl;
    ScreenTransition *screenTransition;
    SaveManager *saveManager;
    QString m_selectedClass;
    QWidget *m_currentScene = nullptr;
    QString m_currentSceneName;
};

#endif // MAINWINDOW_H