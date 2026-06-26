#include "rule.h"
#include "ui_rule.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>

rule::rule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::rule)
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/init_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    this->setWindowTitle("游戏简介");

    ui->textEdit->setReadOnly(true);
    ui->textEdit->setHtml(
        "<style>body { color: #1a1a2e; }</style>"
        "<h1 align='center'>《 匹 夫 》</h1>"
        "<p align='center'><i>天下兴亡，匹夫有责。该怂怂，该狂狂。</i></p>"
        "<hr>"

        "<h2>背景</h2>"
        "<p>南柯大陆，一片被「元力」浸染的土地。"
        "千年前天地崩裂，元力从地脉深处涌出，改变了万物法则。"
        "有人因此觉醒异能、开山裂石；有人被元力反噬、化为荒野兽魔。</p>"
        "<p>时值南柯历三百二十七年秋。玄铁城领主萧正阳封锁城关，断绝外界往来。"
        "黑风洞山贼趁乱猖獗，桃源村首当其冲。"
        "一个自称匹夫的小人物，即将踏上属于他的道路。</p>"

        "<h2>五大职业</h2>"
        "<p><b>战士</b> —— 凭血肉之躯碾碎一切阻碍。正面硬撼，以威压震慑对手。</p>"
        "<p><b>刺客</b> —— 在阴影中裁决生死。暗杀无形，一击致命不留痕迹。</p>"
        "<p><b>法师</b> —— 咒语既出，凡人只能仰望天灾。洞悉元力之秘，追寻真相。</p>"
        "<p><b>富商</b> —— 钱能通神，买不到的就雇人抢。撒币买命，以情报撬动乾坤。</p>"
        "<p><b>盗贼</b> —— 没有锁打不开，没有路逃不掉。潜行于夹缝，游走于无形。</p>"
        "<p><i>不同职业在剧情中拥有专属选项和独特剧情线。</i></p>"

        "<h2>五大场景</h2>"
        "<p><b>桃源村</b> —— 南境边陲的淳朴小村，故事开始的地方。</p>"
        "<p><b>幽暗密林</b> —— 巨木遮天的原始森林，山贼出没之地。</p>"
        "<p><b>黑风洞</b> —— 山贼大本营，千年妖骸深埋其中。</p>"
        "<p><b>青石镇</b> —— 南境最大的镇落，各方势力盘踞。</p>"
        "<p><b>玄铁城</b> —— 南境的心脏，最终对决的舞台。</p>"
        "<p><i>每个场景可自由探索，不同选择指向不同结局。</i></p>"

        "<h2>双主线</h2>"
        "<p><b>侠道</b> —— 助村民剿山贼，救回人质，揭露阴谋，保境安民。</p>"
        "<p><b>枭雄</b> —— 与山贼合作，渗透镇落，夺取城池，取而代之。</p>"
        "<p><i>同一职业在两条主线上均有专属结局。</i></p>"

        "<h2>操作</h2>"
        "<p>· 点击选项按钮推进剧情，不同选择导向不同分支</p>"
        "<p>· 点击「移动」自由选择目的地，任意切换五大场景</p>"
        "<p>· 点击「返回主菜单」回到开始界面</p>"
        "<p>· 退出时可保存进度，下次通过「读取存档」继续</p>"

        "<hr>"
        "<p align='center'><i>乱世之中，一个小人物——可以改变一切。</i></p>"
    );
}

rule::~rule()
{
    delete ui;
}

void rule::on_rule_return_clicked(bool checked)
{
    Q_UNUSED(checked);

    emit Return();
    this->hide();
}