#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "character.h"
#include "rule.h"
#include "village.h"
#include "forest.h"
#include "cave.h"
#include "town.h"
#include "city.h"
#include "savemanager.h"
#include <QPixmap>
#include <QPalette>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "musicmanager.h"
#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , characterwindow(nullptr)
    , rulewindow(nullptr)
    , villagewindow(nullptr)
    , forestwindow(nullptr)
    , cavewindow(nullptr)
    , townwindow(nullptr)
    , citywindow(nullptr)
    , screenControl(new ScreenControl(this))
    , screenTransition(new ScreenTransition(this))
    , saveManager(new SaveManager())
    , m_selectedClass("")
    , m_currentSceneName("")
{
    ui->setupUi(this);

    // 设置背景图
    QPixmap pixmap(":/image/asset/init_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "init");

    screenControl->setupTextEdit(ui->textEdit);
    setupMainText();
    connect(screenControl, &ScreenControl::returnToParent, this, &MainWindow::Onreturn);//
    screenControl->resetStory();
}

void MainWindow::setupMainText()
{
    QMap<int, StoryPage> pages;

    StoryPage page0;
    page0.text = "\n\n\n\n\n"
                 "      《 匹 夫 》\n\n"
                 "    天下兴亡，匹夫有责。\n"
                 "    该怂怂，该狂狂。\n\n"
                 "    在这片名为\"南柯\"的大陆上，\n"
                 "    有人以血肉碾碎荆棘，\n"
                 "    有人在阴影中裁决生死，\n"
                 "    有人借咒语俯瞰众生，\n"
                 "    有人使金钱撬动乾坤，\n"
                 "    也有人凭巧手游走于缝隙之间。\n\n"
                 "    而你——\n\n"
                 "    你选择成为谁？";
    pages.insert(0, page0);

    screenControl->setStories(pages);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Onreturn()
{
    if (m_currentScene) {
        MusicManager::instance()->stop(m_currentScene);
        m_currentScene = nullptr;
    }
    MusicManager::instance()->play(this, "init");
    screenTransition->showScreen(this);
}

//  开始游戏 → 桃源村
void MainWindow::on_button1_clicked(bool checked)
{
    Q_UNUSED(checked);

    // 如果没有选择角色,跳转到角色创建
    if (m_selectedClass.isEmpty())
    {
        on_create_character_clicked(checked);
        return;
    }

    //  有角色,进入桃源村
    if (!villagewindow) {
        villagewindow = new village();
        connect(villagewindow, &village::Return, this, &MainWindow::Onreturn);
        connect(villagewindow, &village::goToForest, this, &MainWindow::onGoToForest);
        connect(villagewindow, &village::navigateTo, this, &MainWindow::onNavigateTo);
    }

    villagewindow->setPlayerClass(m_selectedClass);
    m_currentScene = villagewindow;
    m_currentSceneName = "village";
    MusicManager::instance()->stop(this);
    screenTransition->goToScreen(this, villagewindow);
}

void MainWindow::on_create_character_clicked(bool checked)
{
    Q_UNUSED(checked);

    if(!characterwindow)
    {
        characterwindow = new character();
        //连接character传入角色信息
        connect(characterwindow, &character::classSelected, this, &MainWindow::onClassSelected);
        connect(characterwindow, &character::Return, this, &MainWindow::Onreturn);
    }

    MusicManager::instance()->stop(this);
    screenTransition->goToScreen(this, characterwindow);
}

void MainWindow::onClassSelected(QString className)
{
    //传入角色信息
    m_selectedClass = className;

    if (characterwindow) {
        MusicManager::instance()->stop(characterwindow);
        screenTransition->hideScreen(characterwindow);
    }

    // 直接进入桃源村
    if (!villagewindow) {
        villagewindow = new village();
        connect(villagewindow, &village::Return, this, &MainWindow::Onreturn);
        connect(villagewindow, &village::goToForest, this, &MainWindow::onGoToForest);
        connect(villagewindow, &village::navigateTo, this, &MainWindow::onNavigateTo);
    }

    villagewindow->setPlayerClass(className);//角色信息给到village
    m_currentScene = villagewindow;
    m_currentSceneName = "village";
    screenTransition->showScreen(villagewindow);
}

void MainWindow::startGameWithClass(QString className)
{
    m_selectedClass = className;
    on_button1_clicked(false);
}

void MainWindow::on_rule_button_clicked(bool checked)
{
    Q_UNUSED(checked);

    if(!rulewindow)
    {
        rulewindow = new rule();
        connect(rulewindow, &rule::Return, this, &MainWindow::Onreturn);
    }

    MusicManager::instance()->stop(this);
    screenTransition->goToScreen(this, rulewindow);
}

void MainWindow::on_exit_button_clicked(bool checked)
{
    Q_UNUSED(checked);

    // 只有已选择角色且在游戏中时才询问存档
    if (m_selectedClass.isEmpty() || m_currentSceneName.isEmpty()) {
        exit(0);
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "存档", "是否保存当前游戏进度？",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        return;
    }

    if (reply == QMessageBox::Yes) {
        bool ok;
        QString name = QInputDialog::getText(
            this, "存档", "请输入存档名称：",
            QLineEdit::Normal, "", &ok);

        if (ok && !name.isEmpty()) {
            if (saveManager->saveExists(name)) {
                QMessageBox::StandardButton overwrite = QMessageBox::question(
                    this, "覆盖确认",
                    "存档 \"" + name + "\" 已存在，是否覆盖？",
                    QMessageBox::Yes | QMessageBox::No);
                if (overwrite != QMessageBox::Yes) {
                    return;
                }
            }
            saveManager->saveGame(name, m_selectedClass, m_currentSceneName);
        }
    }

    exit(0);
}

void MainWindow::on_memory_button_clicked(bool checked)
{
    Q_UNUSED(checked);

    QStringList saves = saveManager->listSaves();
    if (saves.isEmpty()) {
        QMessageBox::information(this, "读取存档", "没有找到任何存档。");
        return;
    }

    // 构建存档选择对话框
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("读取存档");
    dlg->setFixedSize(400, 350);
    dlg->setStyleSheet("background-color: #1a1a2e; color: #e0d5c1;");

    QVBoxLayout *layout = new QVBoxLayout(dlg);

    QLabel *title = new QLabel("—— 选择存档 ——", dlg);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-family: 楷体; font-size: 18px; font-weight: bold; background: transparent;");
    layout->addWidget(title);

    QListWidget *list = new QListWidget(dlg);
    list->setStyleSheet(
        "QListWidget { background-color: #2a2a3e; color: #e0d5c1; font-family: 楷体; font-size: 16px; border: 2px solid #4a3f5c; }"
        "QListWidget::item { padding: 8px; }"
        "QListWidget::item:selected { background-color: #4a7a9c; }"
    );
    for (const QString& s : saves) {
        QString info = saveManager->getSaveInfo(s);
        list->addItem(s + "\n" + info);
    }
    layout->addWidget(list);

    QDialogButtonBox *btnBox = new QDialogButtonBox(dlg);
    QPushButton *loadBtn = new QPushButton("读取", dlg);
    QPushButton *deleteBtn = new QPushButton("删除", dlg);
    QPushButton *cancelBtn = new QPushButton("取消", dlg);
    QString btnStyle = "QPushButton { background-color: #4a7a9c; color: #ffffff; font-family: 楷体; font-size: 16px; font-weight: bold; border: 3px solid #2a4a6a; padding: 8px 16px; } QPushButton:hover { background-color: #5a8aac; }";
    loadBtn->setStyleSheet(btnStyle);
    deleteBtn->setStyleSheet(btnStyle);
    cancelBtn->setStyleSheet(btnStyle);
    btnBox->addButton(loadBtn, QDialogButtonBox::AcceptRole);
    btnBox->addButton(deleteBtn, QDialogButtonBox::DestructiveRole);
    btnBox->addButton(cancelBtn, QDialogButtonBox::RejectRole);
    layout->addWidget(btnBox);

    QObject::connect(loadBtn, &QPushButton::clicked, dlg, [&]() {
        int row = list->currentRow();
        if (row >= 0 && row < saves.size()) {
            QString name = saves[row];
            QJsonObject data = saveManager->loadGame(name);
            if (!data.isEmpty()) {
                m_selectedClass = data["playerClass"].toString();
                QString scene = data["currentScene"].toString();
                dlg->accept();
                // 进入游戏到对应场景
                onNavigateTo(scene);
            }
        }
    });

    QObject::connect(deleteBtn, &QPushButton::clicked, dlg, [&]() {
        int row = list->currentRow();
        if (row >= 0 && row < saves.size()) {
            QString name = saves[row];
            QMessageBox::StandardButton confirm = QMessageBox::question(
                dlg, "删除确认", "确定要删除存档 \"" + name + "\" 吗？",
                QMessageBox::Yes | QMessageBox::No);
            if (confirm == QMessageBox::Yes) {
                saveManager->deleteSave(name);
                dlg->reject();  // 关闭对话框，用户可重新打开
            }
        }
    });

    QObject::connect(cancelBtn, &QPushButton::clicked, dlg, &QDialog::reject);

    dlg->exec();
    dlg->deleteLater();
}

void MainWindow::onGoToForest()
{
    if (!forestwindow) {
        forestwindow = new forest();
        connect(forestwindow, &forest::Return, this, &MainWindow::Onreturn);
        connect(forestwindow, &forest::navigateTo, this, &MainWindow::onNavigateTo);
    }
    forestwindow->setPlayerClass(m_selectedClass);
    m_currentScene = forestwindow;
    m_currentSceneName = "forest";
    MusicManager::instance()->stop(villagewindow);
    screenTransition->goToScreen(villagewindow, forestwindow);
}

void MainWindow::onNavigateTo(const QString& sceneName)
{
    QWidget *target = nullptr;

    if (sceneName == "village") {
        if (!villagewindow) {
            villagewindow = new village();
            connect(villagewindow, &village::Return, this, &MainWindow::Onreturn);
            connect(villagewindow, &village::goToForest, this, &MainWindow::onGoToForest);
            connect(villagewindow, &village::navigateTo, this, &MainWindow::onNavigateTo);
        }
        villagewindow->setPlayerClass(m_selectedClass);
        villagewindow->resetStory();
        target = villagewindow;
    } else if (sceneName == "forest") {
        if (!forestwindow) {
            forestwindow = new forest();
            connect(forestwindow, &forest::Return, this, &MainWindow::Onreturn);
            connect(forestwindow, &forest::navigateTo, this, &MainWindow::onNavigateTo);
        }
        forestwindow->setPlayerClass(m_selectedClass);
        target = forestwindow;
    } else if (sceneName == "cave") {
        if (!cavewindow) {
            cavewindow = new cave();
            connect(cavewindow, &cave::Return, this, &MainWindow::Onreturn);
            connect(cavewindow, &cave::navigateTo, this, &MainWindow::onNavigateTo);
        }
        cavewindow->setPlayerClass(m_selectedClass);
        cavewindow->resetStory();
        target = cavewindow;
    } else if (sceneName == "town") {
        if (!townwindow) {
            townwindow = new town();
            connect(townwindow, &town::Return, this, &MainWindow::Onreturn);
            connect(townwindow, &town::navigateTo, this, &MainWindow::onNavigateTo);
        }
        townwindow->setPlayerClass(m_selectedClass);
        townwindow->resetStory();
        target = townwindow;
    } else if (sceneName == "city") {
        if (!citywindow) {
            citywindow = new city();
            connect(citywindow, &city::Return, this, &MainWindow::Onreturn);
            connect(citywindow, &city::navigateTo, this, &MainWindow::onNavigateTo);
        }
        citywindow->setPlayerClass(m_selectedClass);
        citywindow->resetStory();
        target = citywindow;
    }

    if (target) {
        if (m_currentScene) MusicManager::instance()->stop(m_currentScene);
        MusicManager::instance()->play(target, sceneName);
        screenTransition->goToScreen(m_currentScene, target);
        m_currentScene = target;
        m_currentSceneName = sceneName;
    }
}