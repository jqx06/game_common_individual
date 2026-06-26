#ifndef FUNCTION_H
#define FUNCTION_H

#include <QWidget>
#include <QTimer>
#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QStringList>

class ScreenTransition : public QObject
{
    Q_OBJECT

public:
    explicit ScreenTransition(QObject *parent = nullptr);

    void goToScreen(QWidget *currentScreen, QWidget *targetScreen);
    void goBack(QWidget *currentScreen, QWidget *parentScreen);
    void hideScreen(QWidget *screen);
    void showScreen(QWidget *screen);
};

struct StoryChoice {
    QString text;
    int targetPage;
    QString sceneAction;
};

struct StoryPage {
    QString text;
    QVector<StoryChoice> choices;
};

class ScreenControl : public QObject
{
    Q_OBJECT

public:
    explicit ScreenControl(QObject *parent = nullptr);

    void setupTextEdit(QTextEdit *textEdit);
    void setStories(const QMap<int, StoryPage> &pages);
    void setButtons(QPushButton *btn1, QPushButton *btn2 = nullptr, QPushButton *btn3 = nullptr,
                    QPushButton *btn4 = nullptr, QPushButton *btn5 = nullptr,
                    QPushButton *btn6 = nullptr, QPushButton *btn7 = nullptr,
                    QPushButton *btn8 = nullptr, QPushButton *btn9 = nullptr);

    // 核心功能
    void startStory(int pageIndex);
    void resetStory();
    void goToPage(int targetPage);
    void updateText();

    // 按钮控制
    void showChoices();
    void hideAllChoiceButtons();
    void onChoiceClicked(int choiceIndex);
    int getCurrentPage() const { return currentPage; }

signals:
    void returnToParent();
    void pageChanged(int newPage);
    void sceneActionRequested(const QString& action);

private:
    QTextEdit *textEdit;
    QMap<int, StoryPage> pages;

    QVector<QPushButton*> choiceButtons;

    QTimer *timer;
    int currentPage;
    int currentIndex;
    bool isTextComplete;
    bool isWaitingForChoice;
    int choiceCount;
};

#endif // FUNCTION_H