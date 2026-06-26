//这是当时一个测试文件
#include "secondwindow.h"
#include "ui_secondwindow.h"
#include <QPalette>
#include <QPixmap>

Secondwindow::Secondwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Secondwindow)
    , screenTransition(new ScreenTransition(this))
    , screenControl(new ScreenControl(this))
{
    ui->setupUi(this);

    // ======== 设置背景图 ========
    /*QPixmap pixmap(":/image/asset/background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);*/

    // ======== 使用 ScreenControl 设置 textEdit ========
    screenControl->setupTextEdit(ui->textEdit);

    // ======== 使用 ScreenControl 设置按钮 ========
    screenControl->setButtons(ui->button3, ui->button4, ui->button5);

    // ======== 初始化故事数据 ========
    setupStories();

    // ======== 连接返回信号 ========
    connect(screenControl, &ScreenControl::returnToParent,
            this, &Secondwindow::onReturnToParent);

    // ======== 启动第一页 ========
    screenControl->resetStory();
}

Secondwindow::~Secondwindow()
{
    delete ui;
}

void Secondwindow::setPlayerClass(const QString& className)
{
    if (pages.isEmpty()) {
        return;  // 如果没有故事数据，直接返回
    }

    QString prefix;
    if (className == "战士") {
        prefix = "勇敢的战士，你的剑已准备好！\n\n";
    } else if (className == "法师") {
        prefix = "智慧的法师，魔法在指尖流动！\n\n";
    } else if (className == "富商") {
        prefix = "精明的富商，金币已准备好！\n\n";
    } else if (className == "盗贼") {
        prefix = "敏捷的盗贼，阴影已让路！\n\n";
    } else if (className == "刺客") {
        prefix = "致命的刺客，目标已在视野中！\n\n";
    } else {
        prefix = "冒险者，欢迎来到森林！\n\n";
    }

    // 修改第一页的故事内容
    pages[0].text = prefix + pages[0].text;

    // 重新设置故事数据到 ScreenControl
    screenControl->setStories(pages);
    screenControl->resetStory();
}

void Secondwindow::setupStories()
{
    pages.clear();  // 清空

    // ---- 第 0 页：故事开头 ----
    StoryPage page0;
    page0.text = "在时间还未被记载的年代，这片森林便已存在。\n\n"
                 "它的根须扎入大地的心脏，它的树冠触碰着星辰的边缘。\n\n"
                 "这里没有道路，只有无数生灵踏出的足迹；"
                 "这里没有时钟，只有光影在林间移动的刻度。\n\n"
                 "传说，当月光穿透三棵古橡树的缝隙，"
                 "洒落在那块被遗忘的界石上时，森林会向内心纯净者低语。\n\n"
                 "你站在这片古老森林的入口，面前有两条路……\n\n"
                 "你将选择哪一条？";
    page0.choices.push_back({"走向青石空地", 1});
    page0.choices.push_back({"走向密林深处", 2});
    pages.insert(0, page0);

    // ---- 第 1 页：主线 - 青石空地 ----
    StoryPage page1;
    page1.text = "你穿过密密的灌木丛，来到了一片被月光照亮的空地。\n\n"
                 "空地中央有一块巨大的青石，上面刻满了古老的符文。\n\n"
                 "那些符文在月光下隐隐发光，仿佛有生命一般。\n\n"
                 "你伸出手，触碰那冰冷的石面，"
                 "一股暖流瞬间涌遍全身。\n\n"
                 "你明白了——这片森林一直在等待着你。\n\n"
                 "青石上的符文开始发光，"
                 "你感到内心深处有什么东西被唤醒了……";
    page1.choices.push_back({"继续聆听森林的故事", 3});
    page1.choices.push_back({"回到岔路口", 0});
    pages.insert(1, page1);

    // ---- 第 2 页：分支 - 密林深处 ----
    StoryPage page2;
    page2.text = "你循着一条隐秘的小径，向森林更深处走去。\n\n"
                 "这条路从未被记载在任何地图上，"
                 "两旁的树木越来越密，光线越来越暗。\n\n"
                 "走了不知多久，你看到前方有一座古老的石桥，"
                 "桥下是一条闪着银光的地下河。\n\n"
                 "你听到河水在低语，仿佛在呼唤你的名字……";
    page2.choices.push_back({"走过石桥", 4});
    page2.choices.push_back({"回到岔路口", 0});
    pages.insert(2, page2);

    // ---- 第 3 页：主线结局 ----
    StoryPage page3;
    page3.text = "你闭上双眼，感受着森林的脉搏。\n\n"
                 "青石上的符文化作一道道金光，融入你的身体。\n\n"
                 "你获得了森林的祝福与认可，"
                 "成为了这片古老土地的守护者。\n\n"
                 "从此以后，每当月光洒落在这片森林，"
                 "你都能听到那些古老树木的低语……\n\n"
                 "—— 主线故事 · 完 ——";
    page3.choices.push_back({"回到现实世界", -1});
    pages.insert(3, page3);

    // ---- 第 4 页：分支结局 ----
    StoryPage page4;
    page4.text = "你深吸一口气，踏上了那座古老的石桥。\n\n"
                 "石桥的尽头，是一个巨大的地下洞穴。\n\n"
                 "洞穴的墙壁上布满了发光的晶体，"
                 "将整个空间照得如同白昼。\n\n"
                 "在洞穴最深处，你发现了一个古老的宝箱。\n\n"
                 "你打开宝箱——里面没有黄金，没有珠宝，\n"
                 "只有一面铜镜。\n\n"
                 "你看向镜中，看到的却是这片森林最初的模样……\n\n"
                 "—— 分支故事 · 完 ——";
    page4.choices.push_back({"回到现实世界", -1});
    pages.insert(4, page4);

    // 设置故事数据到 ScreenControl
    screenControl->setStories(pages);
}

void Secondwindow::resetStory()
{
    screenControl->resetStory();
}

void Secondwindow::onReturnToParent()
{
    emit Return();
    // 使用 ScreenTransition 隐藏当前窗口
    screenTransition->hideScreen(this);
}

void Secondwindow::on_button2_clicked(bool checked)
{
    Q_UNUSED(checked);
    emit Return();
    screenTransition->hideScreen(this);
}

void Secondwindow::on_button3_clicked(bool checked)
{
    Q_UNUSED(checked);
    screenControl->onChoiceClicked(0);
}

void Secondwindow::on_button4_clicked(bool checked)
{
    Q_UNUSED(checked);
    screenControl->onChoiceClicked(1);
}

void Secondwindow::on_button5_clicked(bool checked)
{
    Q_UNUSED(checked);
    screenControl->onChoiceClicked(2);
}