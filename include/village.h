#ifndef VILLAGE_H
#define VILLAGE_H

#include <QWidget>
#include "function.h"

namespace Ui {
class village;
}

class village : public QWidget
{
    Q_OBJECT

public:
    explicit village(QWidget *parent = nullptr);
    ~village();

    void resetStory();
    void setPlayerClass(const QString& className);

signals:
    void Return();
    void goToForest();
    void navigateTo(const QString& sceneName);

private slots:
    void on_button_back_clicked(bool checked);
    void on_button_next_clicked(bool checked);
    void on_button3_clicked(bool checked);
    void on_button4_clicked(bool checked);
    void on_button5_clicked(bool checked);
    void on_button6_clicked(bool checked);
    void on_button7_clicked(bool checked);
    void onReturnToParent();
    void onSceneActionRequested(const QString& action);

private:
    void rebuildStory();
    void track(const QString& key, const QVariant& val);
    QVariant st(const QString& key) const;
    void refreshHub();

    Ui::village *ui;
    ScreenTransition *screenTransition;
    ScreenControl *screenControl;
    QMap<int, StoryPage> pages;
    QString playerClass;
    QMap<QString, QVariant> m_state;
};

#endif // VILLAGE_H