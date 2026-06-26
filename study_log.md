# 文字冒险游戏 — Qt C++ 技术总结

---

## 一、界面跳转（ScreenTransition）

**文件**：`function.h` + `function.cpp`

所有场景窗口只创建一次，切换时只 hide/show，不销毁。

```cpp
class ScreenTransition : public QObject {
public:
    void goToScreen(QWidget *current, QWidget *target); // 隐藏当前，显示目标
    void hideScreen(QWidget *screen);
    void showScreen(QWidget *screen);
};

// 实现
void ScreenTransition::goToScreen(QWidget *current, QWidget *target) {
    if (current) current->hide();
    if (target)  target->show();
}

// 调用示例
screenTransition->goToScreen(this, villagewindow);         // 主界面→桃源村
screenTransition->goToScreen(m_currentScene, forestwindow); // 场景间跳转
```

---

## 二、文字显示 + 选项分支（ScreenControl）

**文件**：`function.h` + `function.cpp`

### 2.1 数据结构

```cpp
struct StoryChoice {
    QString text;        // 按钮文字
    int targetPage;      // 目标页ID
    QString sceneAction; // 场景跳转
};

struct StoryPage {
    QString text;                     // 页面文本
    QVector<StoryChoice> choices;     // 选项列表
};
```

### 2.2 存储：QMap<int, StoryPage>

```cpp
QMap<int, StoryPage> pages;  // key=页面ID, value=页面内容
```
注：采用QMap避免了数组下标连续而造成查找出错问题，每个page采用11，12，13，21，22，23记录

### 2.3 文字显现（QTimer 逐字输出）

```cpp
void ScreenControl::startStory(int pageIndex) {
    currentPage = pageIndex; currentIndex = 0;
    textEdit->clear(); hideAllChoiceButtons();
    timer->start(5);  // 每5ms输出一个字符
}

void ScreenControl::updateText() {
    const QString& text = pages.value(currentPage).text;
    if (currentIndex < text.length()) //避免越界
    {
        textEdit->insertPlainText(QString(text[currentIndex])); // 逐字添加
        currentIndex++;
    } else {
        timer->stop();
        showChoices();  // 文字完毕弹出选项按钮
    }
}
```

### 2.4 构建故事页

```cpp
void village::rebuildStory() {
    pages.clear();

    //采用录入一页加一页
    StoryPage page0;
    page0.text = "晨光洒在青石板铺就的长街上...";
    page0.choices.push_back({"与村长·覃伯对话", 1});
    page0.choices.push_back({"前往幽暗密林", -2, "forest"});  // sceneAction跳场景
    pages.insert(0, page0);

    StoryPage page1;
    page1.text = "一位七十出头的老者...";
    page1.choices.push_back({"我是来帮忙的。", 11});
    page1.choices.push_back({"路过而已。", 12});
    pages.insert(1, page1);

    //将存储的pages进行构建
    screenControl->setStories(pages);
    screenControl->resetStory();  // 从第0页开始
}
```

### 2.5 按钮处理

```cpp
//  简单的槽函数处理
void village::on_button3_clicked(bool) { screenControl->onChoiceClicked(0); }
void village::on_button4_clicked(bool) { screenControl->onChoiceClicked(1); }
```

### 2.6 跳转逻辑

```cpp
void ScreenControl::onChoiceClicked(int choiceIndex) {
    const StoryChoice& choice = pages.value(currentPage).choices[choiceIndex];
    if (!choice.sceneAction.isEmpty()) {
        emit sceneActionRequested(choice.sceneAction); // 场景跳转优先
        return;
    }
    goToPage(choice.targetPage); // 页面内跳转
}
```

---

## 三、场景间跳转（sceneAction 信号链）

### 3.1 完整信号链

```
剧情选项(sceneAction="forest")
  → ScreenControl::onChoiceClicked
  → emit sceneActionRequested("forest")
  → village::onSceneActionRequested → emit navigateTo("forest")
  → MainWindow::onNavigateTo("forest")
  → 创建/显示 forest 窗口
```

### 3.2 发送端

```cpp
page11.choices.push_back({"前往密林", -2, "forest"});
```

### 3.3 接收端（各场景）

```cpp
// 构造函数中连接信号
connect(screenControl, &ScreenControl::sceneActionRequested,
        this, &village::onSceneActionRequested);

//采用mainwindow统一管理场景跳转，各场景间的跳转统一交给mainwindow处理
void village::onSceneActionRequested(const QString& action) {
    screenTransition->hideScreen(this);
    emit navigateTo(action);  // 发给 MainWindow
}
```

### 3.4 MainWindow 统一处理

```cpp
void MainWindow::onNavigateTo(const QString& sceneName) {
    QWidget *target = nullptr;
    if (sceneName == "village") {
        if (!villagewindow) { villagewindow = new village(); ... }
        villagewindow->setPlayerClass(m_selectedClass);
        villagewindow->resetStory();
        target = villagewindow;
    } else if (sceneName == "forest") { ... }

    if (target) {
        MusicManager::instance()->stop(m_currentScene);
        MusicManager::instance()->play(target, sceneName);
        screenTransition->goToScreen(m_currentScene, target);
        m_currentScene = target;
    }
}
```

---

## 四、移动按钮（自由切换场景）

### 4.1 Travel 对话框

```cpp
class travel : public QDialog {
signals:
    void destinationSelected(const QString& sceneName);
private slots:
    void on_button_village_clicked() { selectAndClose("village"); }
    void on_button_forest_clicked()  { selectAndClose("forest"); }
};
```

### 4.2 调用

```cpp
void village::on_button_next_clicked(bool) {
    travel *dlg = new travel(this);
    connect(dlg, &travel::destinationSelected, this, &village::navigateTo);
    dlg->exec();
}
```

### 4.3 移动按钮限制

```cpp
// 构造函数中隐藏
ui->button_next->setVisible(false);
// button handler中解锁
if (cur == 1) { ui->button_next->setVisible(true); }  // 见覃伯后
```

---

## 五、职业定制 + 状态追踪

### 5.1 职业传递链路

```
character::classSelected("战士")
  → MainWindow::m_selectedClass = "战士"
  → villagewindow->setPlayerClass("战士")
  → rebuildStory() 中所有 if (playerClass == "战士") 生效
```

### 5.2 状态追踪

```cpp
QMap<QString, QVariant> m_state;

void track(const QString& k, const QVariant& v) { m_state[k] = v; }
QVariant st(const QString& k) const { return m_state.value(k); }

// 在button handler中记录
void village::on_button3_clicked(bool) {
    int cur = screenControl->getCurrentPage();
    if (cur == 1) { track("qinbo_choice", 11); }
    screenControl->onChoiceClicked(0);
}
```

### 5.3 Hub页动态刷新（回到村口时选项变化）

```cpp
void village::refreshHub() {
    StoryPage& p0 = pages[0]; p0.choices.clear();
    p0.choices.push_back({"与村长·覃伯对话", 1});
    if (st("qinbo_choice").toInt() == 11)
        p0.choices.push_back({"前往密林（已接委托）", -2, "forest"});
    if (st("got_zhuang_sword").toBool())
        p0.choices.push_back({"握着庄叔的旧刀，前往密林", -2, "forest"});
}
```

### 5.4 职业分支（NPC对话）

```cpp
StoryPage page1;
if (playerClass == "战士") {
    page1.text = "你看着他。两个战士在动手之前，都会先看对方的眼睛...";
    page1.choices.push_back({"我是来带她走的。", 11});
} else if (playerClass == "刺客") {
    page1.text = "四步。从你现在的位置到他后颈，最多四步...";
    page1.choices.push_back({"(沉默评估距离)", 14});
} 
pages.insert(1, page1);
```

---