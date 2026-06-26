#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include "function.h"

namespace Ui {
class Secondwindow;
}

class Secondwindow : public QWidget
{
    Q_OBJECT

public:
    explicit Secondwindow(QWidget *parent = nullptr);
    ~Secondwindow();
    void resetStory();
    void setPlayerClass(const QString& className);

signals:
    void Return();

private slots:
    void on_button2_clicked(bool checked);      // 返回主界面
    void on_button3_clicked(bool checked);      // 选项1
    void on_button4_clicked(bool checked);      // 选项2
    void on_button5_clicked(bool checked);      // 选项3
    void onReturnToParent();                    // 接收 ScreenControl 返回信号

private:
    void setupStories();                        // 初始化故事数据

    Ui::Secondwindow *ui;
    ScreenTransition *screenTransition;         // 跳转管理
    ScreenControl *screenControl;               // 文本和按钮控制
    QMap<int, StoryPage> pages;
};

#endif // SECONDWINDOW_H