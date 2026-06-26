#include "function.h"
#include <QTextCursor>

// ScreenTransition 实现

ScreenTransition::ScreenTransition(QObject *parent): QObject(parent){}

void ScreenTransition::goToScreen(QWidget *currentScreen, QWidget *targetScreen)
{
    if (currentScreen) {
        currentScreen->hide();
    }
    if (targetScreen) {
        targetScreen->show();
    }
}

void ScreenTransition::goBack(QWidget *currentScreen, QWidget *parentScreen)
{
    if (currentScreen) {
        currentScreen->hide();
    }
    if (parentScreen) {
        parentScreen->show();
    }
}

void ScreenTransition::hideScreen(QWidget *screen)
{
    if (screen) {
        screen->hide();
    }
}

void ScreenTransition::showScreen(QWidget *screen)
{
    if (screen) {
        screen->show();
    }
}

// ScreenControl 实现

ScreenControl::ScreenControl(QObject *parent)
    : QObject(parent)
    , textEdit(nullptr)
    , timer(new QTimer(this))
    , currentPage(0)
    , currentIndex(0)
    , isTextComplete(false)
    , isWaitingForChoice(false)
    , choiceCount(0)
{
    connect(timer, &QTimer::timeout, this, &ScreenControl::updateText);
}

void ScreenControl::setupTextEdit(QTextEdit *edit)
{
    textEdit = edit;
    if (textEdit) {
        textEdit->setReadOnly(true);
        textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    }
}

void ScreenControl::setStories(const QMap<int, StoryPage> &pageData)
{
    pages = pageData;
}

void ScreenControl::setButtons(QPushButton *btn1, QPushButton *btn2, QPushButton *btn3,
                               QPushButton *btn4, QPushButton *btn5,
                               QPushButton *btn6, QPushButton *btn7,
                               QPushButton *btn8, QPushButton *btn9)
{
    choiceButtons.clear();

    if (btn1) choiceButtons.append(btn1);
    if (btn2) choiceButtons.append(btn2);
    if (btn3) choiceButtons.append(btn3);
    if (btn4) choiceButtons.append(btn4);
    if (btn5) choiceButtons.append(btn5);
    if (btn6) choiceButtons.append(btn6);
    if (btn7) choiceButtons.append(btn7);
    if (btn8) choiceButtons.append(btn8);
    if (btn9) choiceButtons.append(btn9);

    hideAllChoiceButtons();
}

void ScreenControl::hideAllChoiceButtons()
{
    for (QPushButton* btn : choiceButtons) {
        if (btn)
            btn->setVisible(false);
    }
}

void ScreenControl::showChoices()
{
    hideAllChoiceButtons();

    if (!pages.contains(currentPage)) return;

    const QVector<StoryChoice>& choices = pages.value(currentPage).choices;
    choiceCount = choices.size();

    int maxDisplay = qMin(choiceCount, choiceButtons.size());
    for (int i = 0; i < maxDisplay; ++i) {
        if (choiceButtons[i]) {
            choiceButtons[i]->setText(choices[i].text);
            choiceButtons[i]->setVisible(true);
        }
    }

    isWaitingForChoice = true;
}

void ScreenControl::startStory(int pageIndex)
{
    if (pageIndex == -1) {
        emit returnToParent();
        return;
    }

    if (!pages.contains(pageIndex)) {
        return;
    }

    if (timer->isActive()) {
        timer->stop();
    }

    currentPage = pageIndex;
    currentIndex = 0;
    isTextComplete = false;
    isWaitingForChoice = false;

    if (textEdit) {
        textEdit->clear();
    }

    hideAllChoiceButtons();
    // 文字速度控制
    timer->start(45);
}

void ScreenControl::resetStory()
{
    if (pages.isEmpty()) {
        return;
    }
    startStory(0);
}

void ScreenControl::goToPage(int targetPage)
{
    if (targetPage == -1) {
        emit returnToParent();
        return;
    }

    if (pages.contains(targetPage)) {
        startStory(targetPage);
    }
}

void ScreenControl::updateText()
{
    if (!textEdit || pages.isEmpty() || !pages.contains(currentPage)) {
        timer->stop();
        return;
    }

    const QString& currentText = pages.value(currentPage).text;

    if (currentIndex < currentText.length()) {
        textEdit->insertPlainText(QString(currentText[currentIndex]));
        currentIndex++;

        QTextCursor cursor = textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        textEdit->setTextCursor(cursor);
    } else {
        timer->stop();
        isTextComplete = true;

        if (!pages.value(currentPage).choices.isEmpty()) {
            showChoices();
        }
    }
}

void ScreenControl::onChoiceClicked(int choiceIndex)
{
    if (!isWaitingForChoice) return;
    if (!pages.contains(currentPage)) return;

    const QVector<StoryChoice>& choices = pages.value(currentPage).choices;
    if (choiceIndex >= 0 && choiceIndex < choices.size()) {
        const StoryChoice& choice = choices[choiceIndex];

        // 场景跳转优先于页面跳转
        if (!choice.sceneAction.isEmpty()) {
            emit sceneActionRequested(choice.sceneAction);
            return;
        }

        goToPage(choice.targetPage);
    }
}
