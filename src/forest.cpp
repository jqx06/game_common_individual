#include "forest.h"
#include "ui_forest.h"
#include "travel.h"
#include "musicmanager.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

forest::forest(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::forest)
    , screenTransition(new ScreenTransition(this))
    , screenControl(new ScreenControl(this))
    , playerClass("")
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/forest_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "forest");

    screenControl->setupTextEdit(ui->textEdit);
    QGraphicsDropShadowEffect *ts = new QGraphicsDropShadowEffect();
    ts->setBlurRadius(6);
    ts->setOffset(3, 3);
    ts->setColor(QColor(0, 0, 0, 180));
    ui->textEdit->setGraphicsEffect(ts);
    screenControl->setButtons(ui->button3, ui->button4, ui->button5,
                              ui->button6, ui->button7);

    ui->button_next->setVisible(false);
    ui->button_next->setText("移动");

    connect(screenControl, &ScreenControl::returnToParent,
            this, &forest::onReturnToParent);
    connect(screenControl, &ScreenControl::sceneActionRequested,
            this, &forest::onSceneActionRequested);

    rebuildStory();
    screenControl->resetStory();
}

forest::~forest()
{
    delete ui;
}

void forest::setPlayerClass(const QString& className)
{
    playerClass = className;
    rebuildStory();
    screenControl->resetStory();
}

void forest::rebuildStory()
{
    pages.clear();

    StoryPage page0;
    page0.text = "巨木参天，遮天蔽日。\n\n"
                 "踏入密林的瞬间，光线像被一只无形的手掐灭了大部分。\n"
                 "只剩下斑驳的碎光，透过层层叠叠的树冠洒下来，\n"
                 "在铺满落叶的地面上画出明明暗暗的纹路。\n\n"
                 "空气潮湿，带着腐叶和苔藓混合的气味。\n"
                 "远处隐约传来不知是风声还是野兽低吼的响动。\n\n"
                 "你脚下踩断了一根枯枝。\n"
                 "\"咔\"的一声在林间回荡——\n"
                 "然后，你听到了别的声音。\n"
                 "不是风声。\n"
                 "是脚步声。\n\n"
                 "从三个方向，由远及近。\n\n"
                 "你看到灌木丛后蹲着一个受伤的猎户，\n"
                 "溪涧对岸的树上似乎还有一个人影。";
    page0.choices.push_back({"与猎户·老赵对话", 1});
    page0.choices.push_back({"走向溪涧对岸（山贼探子）", 2});
    if (playerClass == "富商") {
        page0.choices.push_back({"（富商专属）查看侧翻的马车", 3});
    }
    page0.choices.push_back({"前往黑风洞", -2, "cave"});
    page0.choices.push_back({"返回桃源村", -2, "village"});
    pages.insert(0, page0);

    // ---- 猎户·老赵（按职业定制） ----
    StoryPage page1;
    if (playerClass == "战士") {
        page1.text = "一个四十岁左右的中年猎户，蹲在灌木丛后，攥着一支断箭。\n"
                     "左腿还在渗血，但他握弓的手没有松开。\n\n"
                     "他看到你身上的兵器——眼神亮了一下。\n"
                     "\"练家子！好——听好，前面三百步，五个山贼。\n"
                     "领头那个刀法不弱，疤面的手下。\"\n\n"
                     "他用断箭在地上画了一个简图。\n"
                     "\"正面同时对付五个——你行不行？\n"
                     "要是行，我帮你引开两个。剩下三个归你。\"\n\n"
                     "他在找一个可以并肩作战的人。\n"
                     "他已经一个人在这片林子里撑了太久了。";
        QVector<StoryChoice> c1;
        c1.push_back({"\"不用引——五个我都接了。\"", 11});
        c1.push_back({"\"你腿伤了。先告诉我陷阱的位置。\"", 12});
        c1.push_back({"\"你在这里守了多久？\"", 13});
        c1.push_back({"回到入口", 0});
        page1.choices = c1;
    } else if (playerClass == "刺客") {
        page1.text = "一个四十岁左右的中年猎户，蹲在灌木丛后。\n"
                     "他攥着断箭的手在微微发抖——不是因为害怕，是因为失血。\n\n"
                     "你蹲到他身边时没有发出任何声音。\n"
                     "他转过头看到你的一瞬间——本能地往后缩了半寸。\n"
                     "不是怕敌人。是怕一个他听不到脚步声的人。\n\n"
                     "\"你……什么时候在我旁边的？\"\n\n"
                     "他盯着你看了两秒。然后做了一个决定——信任你。\n"
                     "\"算了。不管你是谁——前面三百步，五个山贼在巡逻。\n"
                     "你要是能摸过去，换岗有三十息的空档。\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"（刺客专属）他们的巡逻规律——精确到息。\"", 14});
        c1.push_back({"\"你的伤——我帮你止血。\"", 13});
        c1.push_back({"回到入口", 0});
        page1.choices = c1;
    } else if (playerClass == "法师") {
        page1.text = "一个四十岁左右的中年猎户，蹲在灌木丛后。\n"
                     "左腿有伤，但你注意到他的伤口边缘有一丝不寻常的蓝色——\n"
                     "那是元力灼烧后未清理干净的残留。\n\n"
                     "\"你是桃源村来的？\"他把你拽到灌木后。\n"
                     "\"别走大路——前面有山贼。还有——\"\n"
                     "他压低声音。\n"
                     "\"林子深处有东西。不是野兽——是某种更老的。\n"
                     "我上个月追鹿的时候走到了一个从来没去过的地方，\n"
                     "那里的石头上刻满了发光的符号。我一碰——\n"
                     "腿就被灼伤了。不是火，是别的什么。\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"符文。你碰了元力符文——能带我去看吗？\"", 11});
        c1.push_back({"\"先处理山贼。符文的事稍后再说。\"", 12});
        c1.push_back({"回到入口", 0});
        page1.choices = c1;
    } else if (playerClass == "富商") {
        page1.text = "一个四十岁左右的中年猎户，蹲在灌木丛后。\n"
                     "他的弓断了，腿上在渗血，但眼神还很清醒。\n\n"
                     "\"你是商人？\"他打量了你的衣着。\n"
                     "\"有吃的吗？有药吗？我有东西跟你换——\n"
                     "上个月我在这林子里捡到一块骨头。\n"
                     "不是动物的——上面刻着字。青石镇的黑市商人说这叫龙骨，\n"
                     "有人愿意出大价钱收。\"\n\n"
                     "他从怀里摸出一个布包，里面是一块泛着幽蓝光泽的骨片。\n"
                     "\"救我一命——这东西归你。\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"成交。\"（给他金疮药和干粮）", 11});
        c1.push_back({"\"龙骨？你从哪捡的——带我去。\"", 13});
        c1.push_back({"回到入口", 0});
        page1.choices = c1;
    } else if (playerClass == "盗贼") {
        page1.text = "一个四十岁左右的中年猎户，蹲在灌木丛后。\n"
                     "他腰间挂着一把猎刀——刀柄上的皮绳编法很特别。\n"
                     "你认得那个编法。那是黑市里用来标记'赃物已出手'的暗结。\n\n"
                     "\"嘘——蹲下！\"他把你拉到灌木后。\n"
                     "\"前面有山贼。但你这种人——应该不怕山贼。\n"
                     "你在看我的刀？\"\n\n"
                     "他低头看了一眼自己腰间的猎刀。\n"
                     "\"这把刀不是偷的——是换的。上个月我用三张狼皮跟一个过路的换的。\n"
                     "那人是黑市的。\"他顿了顿，\"你也是？\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"算是。前面有几个？\"", 11});
        c1.push_back({"\"黑市的人——你从哪条路跟他接头的？\"", 12});
        c1.push_back({"回到入口", 0});
        page1.choices = c1;
    }
    pages.insert(1, page1);

    StoryPage page11;
    page11.text = "老赵上下打量你，眼神里有意外，也有一丝担忧。\n\n"
                  "\"年轻人，光有胆色不够。那领头的山贼叫'疤面'的手下，\n"
                  "刀法是在死人身上练出来的。你可想好了——这不是比武，是搏命。\"";
    page11.choices.push_back({"回到入口", 0});
    pages.insert(11, page11);

    StoryPage page12;
    page12.text = "老赵摇摇头，指了指东西两面。\n\n"
                  "\"绕不开。两边的山路都有山贼设的绊索和陷阱——\n"
                  "踩中了，腿就没了。我这条腿，就是踩中陷阱伤的。\n"
                  "正面走虽然凶险，但反而是唯一能走的路。\"\n\n"
                  "他笑了一声，笑声很苦。\n"
                  "\"这林子的路，二十年前是我带人修的。现在——成了山贼的院子。\"";
    page12.choices.push_back({"回到入口", 0});
    pages.insert(12, page12);

    StoryPage page13;
    page13.text = "老赵嘴角抽了抽，像是想笑又痛得笑不出来。\n\n"
                  "\"不止是伤。他们还抢了我半个月打的猎物，\n"
                  "外加两把猎弓——其中一把是我爹留给我的。\"\n\n"
                  "他顿了顿，看向密林深处的方向，眼神黯淡。\n\n"
                  "\"你要是路过黑风洞——能不能帮我把弓带回来？\n"
                  "弓身上刻了一个'赵'字。你认得出的。\"";
    page13.choices.push_back({"\"我帮你找回来。\"", 0});
    page13.choices.push_back({"回到入口", 0});
    pages.insert(13, page13);

    StoryPage page14;
    page14.text = "老赵一愣，随即压低声音，语速很快。\n\n"
                  "\"五个人，每隔一炷香在林间小路换一次岗。\n"
                  "换岗的时候有一个约三十息的时间窗口——\n"
                  "那段路完全没人盯。他们自己都不知道这个漏洞。\"\n\n"
                  "他看了你一眼。\n"
                  "\"我当了二十年猎户。猎人和猎物最大的区别不是谁厉害——\n"
                  "是谁更有耐心。\"\n\n"
                  "你获得了「巡逻时间表」：此后在密林中遇敌的概率大幅降低。";
    page14.choices.push_back({"回到入口", 0});
    pages.insert(14, page14);

    // ---- 山贼探子（关键分支点） ----
    StoryPage page2;
    page2.text = "你在溪涧边蹲下喝水时，一支箭钉在了你脚边三寸的地方。\n\n"
                 "抬头——对岸的树上蹲着一个精瘦的年轻人，\n"
                 "手里的弓还保持着张开的姿势，嘴角挂着似笑非笑的痞气。\n"
                 "他看起来不超过二十岁，但那双眼睛已经不属于任何少年了。\n\n"
                 "\"喝了我们黑风洞的水，不交点过路费说不过去吧？\"\n\n"
                 "他从树上跳下来，落地轻得像一只猫。\n\n"
                 "\"别紧张。最近老大说要招人。我看你挺有种——\n"
                 "一个人敢穿密林。怎么样？要不要来黑风洞喝一杯？\n"
                 "酒虽然是从桃源村抢的，但——好喝。\"";
    QVector<StoryChoice> choices2;
    choices2.push_back({"\"我对山贼没兴趣。\"", 21});
    choices2.push_back({"\"先听听条件。\"", 22});
    if (playerClass == "富商") {
        choices2.push_back({"\"（富商专属）你们黑风洞招人，一个月给多少？\"", 24});
    }
    if (playerClass == "盗贼") {
        choices2.push_back({"\"（盗贼专属）抢来的酒一定不差，该多抢几坛。\"", 25});
    }
    choices2.push_back({"回到入口", 0});
    page2.choices = choices2;
    pages.insert(2, page2);

    StoryPage page21;
    page21.text = "探子脸色一沉。笑容还挂在脸上，但眼睛已经冷了——\n"
                  "那种冷不是愤怒，而是某种司空见惯的、对拒绝的处置方式。\n\n"
                  "\"那就是不给面子了？行。\"\n\n"
                  "他缓缓从背后取下弓。动作不快，但极其流畅——\n"
                  "取弓、搭箭、拉弦三个动作在呼吸之间完成，\n"
                  "箭头已经对准了你的咽喉。\n\n"
                  "\"这片林子里死过不少人。多你一个，连埋都懒得埋。\"\n\n"
                  "话音未落，弓弦响了。";
    page21.choices.push_back({"——接战——", 211});
    pages.insert(21, page21);

    StoryPage page211;
    page211.text = "弦响的瞬间，你没有后退——你往前冲。\n\n"
                    "箭擦着你的耳廓飞过，带走了几根头发和一瞬尖锐的风声。\n"
                    "探子的瞳孔收缩了——他没想到有人会冲着箭来的方向冲。\n\n"
                    "三步。两步。一步。\n\n"
                    "你进入了他的近身范围。弓在近战中是累赘——\n"
                    "探子立刻弃弓，从腰间拔出一把短刀。\n"
                    "刀光在斑驳的树影下划出一道弧线，直取你的侧颈。\n\n"
                    "与此同时，你听到了两侧灌木丛中的脚步声——\n"
                    "两个山贼喽啰，一左一右，封住了你的退路。\n"
                    "三对一。他们配合默契——一个正面牵制，两个侧翼包抄。\n\n"
                    "你体内的元力开始翻涌。每一根元力触角都在向外伸展，\n"
                    "感知着空气中最细微的波动——\n"
                    "左边那人的刀柄在出汗的手心里滑了一下，\n"
                    "右边那人踩断了一根枯枝，重心偏了半寸，\n"
                    "正面的探子眼神飘了一瞬——他在找你的破绽。\n\n"
                    "但你全身都是破绽。\n"
                    "因为你的注意力不在他们任何一个人身上——\n"
                    "而是在他们三人合围的那道缝隙里。\n"
                    "那道缝隙只有一瞬。但一瞬就够了。";
    page211.choices.push_back({"——出手——", 212});
    pages.insert(211, page211);

    StoryPage page212;
    page212.text = "你在那道缝隙出现之前就动了。\n\n"
                    "不是等你看到它——而是等你算到它。\n"
                    "三人的合围默契有余，速度不足。\n"
                    "左侧喽啰的刀慢了半拍——因为他的刀柄在湿滑的手心里又滑了一下。\n"
                    "就是这半拍。\n\n"
                    "你侧身——不是闪避，是切入。\n"
                    "你的肩膀撞进左侧喽啰的怀里，\n"
                    "手臂顺势沿着他的刀背滑上去，\n"
                    "手指扣住了他的手腕——一拧。\n"
                    "骨头错位的脆响和短刀落地的声音同时响起。\n\n"
                    "你没有停顿。拧腕之后是同侧肘击——\n"
                    "肘尖撞在他的太阳穴上。\n"
                    "力道从腰部的旋转中产生，\n"
                    "沿着脊柱传上来，在肘尖凝成一个锐利的点。\n"
                    "喽啰的眼睛翻白，身体软了下去。\n\n"
                    "你借着肘击的反作用力转身——\n"
                    "右侧喽啰的刀已经劈到了你原先站立的位置，\n"
                    "却只劈中了你转身后扬起的衣角。\n"
                    "你的脚后跟踩在他的脚背上——\n"
                    "他本能地低头。\n"
                    "你的膝盖已经等在他低头的轨迹上了。\n\n"
                    "两声闷响。两个喽啰倒在枯叶堆里。\n\n"
                    "探子没有跑。他握紧了短刀，呼吸粗重。\n"
                    "他的眼神从冷变成了另一种东西——\n"
                    "不是恐惧，是不甘。\n\n"
                    "他的刀尖开始发抖——不是因为害怕，\n"
                    "是因为他握得太紧了。\n\n"
                    "你没有动。你只是看着他。\n"
                    "三息之后，他的刀尖不再抖了。\n"
                    "他松开了手。\n"
                    "短刀落在枯叶上，没有声音。\n\n"
                    "\"……走吧。黑风洞在往北三里。\n"
                    "疤面老大会想见你的。\"\n\n"
                    "他转身消失在密林深处，\n"
                    "弓还在地上，弦还在微微颤动。";
    page212.choices.push_back({"前往黑风洞", -2, "cave"});
    page212.choices.push_back({"回到密林入口", 0});
    pages.insert(212, page212);

    StoryPage page22;
    page22.text = "探子咧嘴一笑，露出被烟熏黄的牙。\n"
                  "他把弓往肩上一扛，转身就走，边走边回头招手。\n\n"
                  "\"痛快！我就喜欢跟不废话的人打交道。\n"
                  "我们疤面老大说了——乱世里，人才最贵。\n"
                  "刀可以买，命可以买，但敢提着刀跟人玩命的——买不到。\"\n\n"
                  "\"跟我走，带你去见老大。至于能不能留下来——看你自己的本事。\"\n\n"
                  "（解锁场景：黑风洞。进入时山贼态度中立。）";
    page22.choices.push_back({"前往黑风洞", -2, "cave"});
    pages.insert(22, page22);

    StoryPage page24;
    page24.text = "探子愣了一下，然后捧着肚子笑得弓都拿不稳了。\n\n"
                  "\"哈哈哈哈！你他娘的还真是个实在人！\n"
                  "我在这里拦了一年多的路人——你是第一个跟我谈工资的。\"\n\n"
                  "笑完之后，他擦了擦眼泪，认真地回答：\n"
                  "\"一个月——看你能抢多少，抽三成给洞里，剩下的都是自己的。\n"
                  "不过你要是能给老大管账……说实话，我们洞里最缺的就是会算账的人。\n"
                  "上次分赃，我跟老六打了三架——就因为谁都算不清。\"\n\n"
                  "（获得额外选项：可花费重金收买探子，跳过黑风洞入口的战斗。）";
    page24.choices.push_back({"回到入口", 0});
    pages.insert(24, page24);

    StoryPage page25;
    page25.text = "探子的笑僵了一下。他盯着你看了两秒，\n"
                  "然后——真正的，发自内心地笑了。笑得比刚才真诚得多。\n\n"
                  "\"你眼真毒。那坛酒确实是桃源村抢的。\n"
                  "怎么，你认识村里的人？要替他们出头？\"\n\n"
                  "\"不。\"你耸耸肩，\"我只是觉得——抢来的酒一定不差，该多抢几坛。\"\n\n"
                  "探子脸上的警惕彻底消失了，取而代之的是找到同类的默契。\n"
                  "他走上来，用胳膊肘撞了撞你。\n\n"
                  "\"好家伙，我就知道你不是那种假正经的人。\n"
                  "走吧，今晚的酒算我的——\n"
                  "顺便给你看看我们洞里新挖的酒窖。\"\n\n"
                  "（盗贼在邪道线上获得额外好感度加成。）";
    page25.choices.push_back({"前往黑风洞", -2, "cave"});
    pages.insert(25, page25);

    // ---- 受伤的商人（富商专属） ----
    StoryPage page3;
    page3.text = "密林深处，一辆侧翻的马车旁，一个穿着锦袍的中年胖子瘫坐在地上。\n"
                 "额头上磕破了一个口子，血混着汗糊了半张脸。\n"
                 "身旁散落着绸缎和瓷器碎片——都是上好的货，但现在只是碎在泥里的垃圾。\n\n"
                 "\"救、救命……我的护卫被打散了，货也毁了七成……\"\n\n"
                 "他看到你腰间的钱袋——那双因为失血而涣散的眼睛，\n"
                 "居然在这时候还亮了一下。\n\n"
                 "\"阁下也是生意人？救我出去——回到青石镇，我必有重谢。\n"
                 "而且我可以告诉你一条值钱的消息，关于玄铁城的——\n"
                 "关于领主身边那位法师的。青玉案。\"";
    page3.choices.push_back({"\"先谈价，再救人。\"", 31});
    page3.choices.push_back({"\"我不要钱。你欠我一个人情。\"", 32});
    page3.choices.push_back({"回到入口", 0});
    pages.insert(3, page3);

    StoryPage page31;
    page31.text = "商人一愣，随即苦笑着从怀里摸出一个沉甸甸的钱袋。\n\n"
                   "\"好、好——这才是生意人。这条情报就是：\n"
                   "青玉案每个月都要从黑市收一批'龙骨'。\n"
                   "没人知道他买这个干什么——\n"
                   "但他在过去三年里，买了整个南境能买到的所有龙骨。\"\n\n"
                   "他顿了顿。\n"
                   "\"我之所以知道——是因为其中有一批，就是从我手上卖出去的。\n"
                   "我把货送到玄铁城的时候，亲眼看到他的高台上堆满了骨头。\n"
                   "不是动物的骨头——是人的。\"";
    page31.choices.push_back({"前往黑风洞", -2, "cave"});
    page31.choices.push_back({"前往青石镇", -2, "town"});
    pages.insert(31, page31);

    StoryPage page32;
    page32.text = "商人愣了一下。然后他慢慢收起了惊恐的表情，\n"
                   "露出了一个商人该有的精明眼神。\n\n"
                   "\"人情？比钱贵。但你说得对——\n"
                   "在这个世道，一个靠谱的人情，比一袋子金币值钱。\"\n\n"
                   "他从怀里掏出一枚铜戒指，上面刻着一个奇怪的符号。\n"
                   "\"到了青石镇，去黑市找老鬼。给他看这枚戒指——\n"
                   "他会给你开一扇别人找不到的门。\"";
    page32.choices.push_back({"前往黑风洞", -2, "cave"});
    page32.choices.push_back({"前往青石镇", -2, "town"});
    pages.insert(32, page32);

    screenControl->setStories(pages);
}

void forest::resetStory()
{
    screenControl->resetStory();
}

void forest::onReturnToParent()
{
    emit Return();
    screenTransition->hideScreen(this);
}

void forest::onSceneActionRequested(const QString& action)
{
    screenTransition->hideScreen(this);
    emit navigateTo(action);
}

void forest::on_button_next_clicked(bool checked)
{
    Q_UNUSED(checked);

    travel *dlg = new travel(this);
    connect(dlg, &travel::destinationSelected, this, &forest::navigateTo);
    connect(dlg, &travel::destinationSelected, dlg, &QDialog::deleteLater);
    dlg->exec();
}

void forest::on_button_back_clicked(bool checked)
{
    Q_UNUSED(checked);
    emit Return();
    screenTransition->hideScreen(this);
}

void forest::on_button3_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 1) track("visited_hunter", true);
    if (cur == 2) { track("scout_response", 21); ui->button_next->setVisible(true); }
    if (cur == 13) track("hunter_promised", true);
    screenControl->onChoiceClicked(0);
    if (screenControl->getCurrentPage() == 0) refreshHub();
}

void forest::on_button4_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 2) { track("scout_response", 22); ui->button_next->setVisible(true); }
    screenControl->onChoiceClicked(1);
    if (screenControl->getCurrentPage() == 0) refreshHub();
}

void forest::on_button5_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 2) track("scout_response", 24); // 富商
    screenControl->onChoiceClicked(2);
    if (screenControl->getCurrentPage() == 0) refreshHub();
}

void forest::on_button6_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 2) track("scout_response", 25); // 盗贼
    screenControl->onChoiceClicked(3);
    if (screenControl->getCurrentPage() == 0) refreshHub();
}

void forest::on_button7_clicked(bool checked)
{
    Q_UNUSED(checked);
    screenControl->onChoiceClicked(4);
}

void forest::track(const QString& k, const QVariant& v) { m_state[k] = v; }
QVariant forest::st(const QString& k) const { return m_state.value(k); }

void forest::refreshHub()
{
    if (!pages.contains(0)) return;
    StoryPage& p0 = pages[0];
    p0.choices.clear();
    p0.choices.push_back({"与猎户·老赵对话", 1});
    p0.choices.push_back({"走向溪涧对岸（山贼探子）", 2});
    if (playerClass == "富商") p0.choices.push_back({"（富商专属）查看侧翻的马车", 3});
    if (st("scout_response").toInt() == 21) p0.choices.push_back({"（已击退探子）前往黑风洞", -2, "cave"});
    if (st("scout_response").toInt() == 22) p0.choices.push_back({"（已入伙）前往黑风洞", -2, "cave"});
    if (st("hunter_promised").toBool()) p0.choices.push_back({"（承诺帮老赵找弓）前往黑风洞", -2, "cave"});
    p0.choices.push_back({"前往黑风洞", -2, "cave"});
    p0.choices.push_back({"返回桃源村", -2, "village"});
}
