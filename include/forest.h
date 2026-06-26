#ifndef FOREST_H
#define FOREST_H

#include <QWidget>
#include "function.h"

namespace Ui {
class forest;
}

class forest : public QWidget
{
    Q_OBJECT

public:
    explicit forest(QWidget *parent = nullptr);
    ~forest();

    void resetStory();
    void setPlayerClass(const QString& className);

signals:
    void Return();
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

    Ui::forest *ui;
    ScreenTransition *screenTransition;
    ScreenControl *screenControl;
    QMap<int, StoryPage> pages;
    QString playerClass;
    QMap<QString, QVariant> m_state;

    void track(const QString& k, const QVariant& v);
    QVariant st(const QString& k) const;
    void refreshHub();
};

#endif // FOREST_H
