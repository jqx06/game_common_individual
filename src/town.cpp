#include "town.h"
#include "ui_town.h"
#include "travel.h"
#include "musicmanager.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

town::town(QWidget *parent)
    : QWidget(parent), ui(new Ui::town),
      screenTransition(new ScreenTransition(this)),
      screenControl(new ScreenControl(this)), playerClass("")
{
    ui->setupUi(this);
    QPixmap pixmap(":/image/asset/town_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "town");

    screenControl->setupTextEdit(ui->textEdit);
    QGraphicsDropShadowEffect *ts = new QGraphicsDropShadowEffect();
    ts->setBlurRadius(6);
    ts->setOffset(3, 3);
    ts->setColor(QColor(0, 0, 0, 180));
    ui->textEdit->setGraphicsEffect(ts);
    screenControl->setButtons(ui->button3, ui->button4, ui->button5, ui->button6, ui->button7);
    ui->button_next->setVisible(false);
    ui->button_next->setText("移动");
    connect(screenControl, &ScreenControl::returnToParent, this, &town::onReturnToParent);
    connect(screenControl, &ScreenControl::sceneActionRequested, this, &town::onSceneActionRequested);
    rebuildStory();
    screenControl->resetStory();
}

town::~town() { delete ui; }

void town::setPlayerClass(const QString& className) { playerClass = className; rebuildStory(); screenControl->resetStory(); }

void town::rebuildStory()
{
    pages.clear();

    StoryPage page0;
    page0.text = "出了密林，天地豁然开朗。\n\n"
                 "青石镇——整片南境最大的镇落。\n"
                 "青石板铺就的大道能并排走四辆马车，\n"
                 "街道两旁是连绵的商铺，幌子在风中翻飞。\n\n"
                 "但仔细看——\n"
                 "镇口的守卫盔甲下藏着兵器，眼睛一刻不停地打量每一个进出的人。\n"
                 "小巷深处，有裹着黑斗篷的人影在低语交易。\n"
                 "广场公告栏上贴满了通缉令。\n\n"
                 "繁华的表象下，暗流涌动。\n\n"
                 "你看到镇长府衙、黑市小巷、练兵场和酒馆。";
    if (playerClass == "战士") {
        page0.text += "\n\n练兵场上的喊杀声吸引了你的注意。你本能地朝那个方向望了一眼。";
        page0.choices.push_back({"★ 前往练兵场——见铁峰", 3});
        page0.choices.push_back({"前往镇长府衙（沈大人）", 1});
        page0.choices.push_back({"前往黑市小巷（老鬼）", 2});
        page0.choices.push_back({"前往酒馆（血狼）", 4});
    } else if (playerClass == "刺客") {
        page0.text += "\n\n你习惯性地扫了一眼镇中最暗的巷子——那里有人在做不该在阳光下做的生意。";
        page0.choices.push_back({"★ 前往黑市小巷——见老鬼", 2});
        page0.choices.push_back({"前往镇长府衙（沈大人）", 1});
        page0.choices.push_back({"前往练兵场（铁峰）", 3});
        page0.choices.push_back({"前往酒馆（血狼）", 4});
    } else if (playerClass == "法师") {
        page0.text += "\n\n镇中心的府衙上悬挂着一面旗帜，旗帜上的元力残留引起了你的注意。";
        page0.choices.push_back({"★ 前往镇长府衙——见沈大人", 1});
        page0.choices.push_back({"前往黑市小巷（老鬼）", 2});
        page0.choices.push_back({"前往练兵场（铁峰）", 3});
        page0.choices.push_back({"前往酒馆（血狼）", 4});
    } else if (playerClass == "富商") {
        page0.text += "\n\n酒馆里传来的斧刃磨刀声——那不是普通的喝酒，那是在谈生意。";
        page0.choices.push_back({"★ 前往酒馆——见血狼", 4});
        page0.choices.push_back({"前往镇长府衙（沈大人）", 1});
        page0.choices.push_back({"前往黑市小巷（老鬼）", 2});
        page0.choices.push_back({"前往练兵场（铁峰）", 3});
    } else if (playerClass == "盗贼") {
        page0.text += "\n\n黑市小巷的角落里，一个金牙老头正在等一个值得他开门的人。";
        page0.choices.push_back({"★ 前往黑市小巷——见老鬼", 2});
        page0.choices.push_back({"前往镇长府衙（沈大人）", 1});
        page0.choices.push_back({"前往练兵场（铁峰）", 3});
        page0.choices.push_back({"前往酒馆（血狼）", 4});
    }
    page0.choices.push_back({"前往玄铁城", -2, "city"});
    page0.choices.push_back({"返回黑风洞", -2, "cave"});
    pages.insert(0, page0);

    // ---- 沈大人 ----
    StoryPage page1;
    page1.text = "镇长府衙大堂之内，一个穿着深蓝色官袍的中年男子坐在案桌后。\n"
                 "面容儒雅，手里摇着一柄折扇，扇面上写着——「天下兴亡」。\n\n"
                 "\"你是从密林方向来的？山贼的事——如何了？\"\n\n"
                 "他合上折扇，眼神里有疲惫，也有一丝期待。";
    page1.choices.push_back({"\"疤面已除。瑶儿姑娘已救出。\"", 11});
    page1.choices.push_back({"\"我跟疤面做了笔交易。\"", 12});
    if (playerClass == "战士") {
        page1.choices.push_back({"\"（战士专属）给我一支小队，我替你清剿残余山贼。\"", 13});
    }
    if (playerClass == "法师") {
        page1.choices.push_back({"\"（法师专属）沈大人，你折扇上的字——谁写的？\"", 14});
    }
    page1.choices.push_back({"回到镇中心", 0});
    pages.insert(1, page1);

    StoryPage page11;
    page11.text = "沈大人猛地站起，折扇\"啪\"地敲在案桌上。\n\n"
                  "\"好！好！来人，备宴！\"\n\n"
                  "他绕过案桌，亲自走到你面前。近看之下，他的眼睛里竟有些红。\n\n"
                  "\"实不相瞒——老夫等一个能剿山贼的人，等了整整一年。\n"
                  "你是第一个让我觉得自己这个镇长不是废物的义士。\"\n\n"
                  "他转身从柜中取出一封信，盖上了青石镇大印。\n"
                  "\"这是推荐信。拿着它去玄铁城，守卫不敢拦。\"";
    page11.choices.push_back({"前往玄铁城", -2, "city"});
    page11.choices.push_back({"回到镇中心", 0});
    pages.insert(11, page11);

    StoryPage page12;
    page12.text = "沈大人沉默了片刻。折扇在手中转了半圈。\n\n"
                  "\"交易？跟山贼做交易——你知道这意味着什么吗？\"\n\n"
                  "他的语气没有愤怒，但比愤怒更沉重。\n\n"
                  "\"意味着你承认了他们的合法性。意味着那些被他们劫杀的行商——\n"
                  "得不到任何公道。\"\n\n"
                  "他叹了口气。\n"
                  "\"但我也知道——这世道，不是非黑即白。\n"
                  "你走吧。在老夫改主意之前。\"";
    page12.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(12, page12);

    StoryPage page13;
    page13.text = "沈大人盯了你三秒，转头对手下道：\n"
                  "\"去找铁峰。告诉他——有人要借兵。\"\n\n"
                  "他重新展开折扇，四个字在烛火下格外清晰。\n"
                  "\"天下兴亡——不是一个人的事。\n"
                  "铁峰的兵从来没借给过外人。但这次——\n"
                  "老夫亲自替你开口。\"";
    page13.choices.push_back({"去找铁峰借兵", 3});
    pages.insert(13, page13);

    StoryPage page14;
    page14.text = "沈大人的手顿住了。那只刚才还在轻摇折扇的手，\n"
                  "忽然就不动了。他盯着你看了五秒。\n\n"
                  "\"……你认识青玉案？\"\n\n"
                  "他压低声音念出这个名字时的语气，不像在说一位宫廷法师，\n"
                  "更像在说一个不该被提起的禁忌之名。\n\n"
                  "\"这把折扇上的字——是他写的。三年前他来青石镇视察。\n"
                  "他走后第三天，我的前两任镇长——一位暴毙，一位坠崖。\"\n\n"
                  "他合上折扇，翻到背面。背面还有一行小字：\n"
                  "「以文清谈，以文欢舞，以文载道」";
    page14.choices.push_back({"前往玄铁城调查青玉案", -2, "city"});
    pages.insert(14, page14);

    // ---- 老鬼（按职业定制） ----
    StoryPage page2;
    if (playerClass == "战士") {
        page2.text = "小巷尽头一扇不起眼的木门。你推开——\n"
                     "里面堆满兽骨、旧书、生锈兵器。\n"
                     "柜台后的瘦老头抬起头，金牙在烛火下闪了一下。\n\n"
                     "\"稀客。上回来的兵——是十年前的事了。\n"
                     "他买了一把刀。后来听说死在黑风洞了。\"\n\n"
                     "他往前探了探身。\n"
                     "\"你身上有铁峰的味道——你去过练兵场了？\n"
                     "那小子还欠我三坛酒钱。不过我不急——\n"
                     "我这里还卖情报。比酒贵，但比命便宜。\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"我要情报——关于玄铁城。\"", 21});
        c2.push_back({"\"铁峰欠你的酒钱——我替他还。\"", 24});
        c2.push_back({"回到镇中心", 0});
        page2.choices = c2;
    } else if (playerClass == "刺客") {
        page2.text = "小巷尽头一扇不起眼的木门。你没走正门——\n"
                     "你翻过后墙，从仓库的天窗钻了进来。\n\n"
                     "柜台后的瘦老头没有抬头。但他的金牙在烛火下闪了一下。\n"
                     "\"天窗上的灰——少了。上次有人擦过。\"\n\n"
                     "他抬起眼皮，看着你。\n"
                     "\"你不走前门。好。走前门的人，要么没钱，要么有鬼。\n"
                     "你两样都没有——你只是习惯。\"\n\n"
                     "他从柜台下拿出一把没有刀鞘的匕首。刀身漆黑。\n"
                     "\"你要的东西——我这里都有。但你得先回答我一个问题：\n"
                     "你要杀人，还是救人？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"都有。先救人，再杀人。\"", 21});
        c2.push_back({"\"我要玄铁城地牢的钥匙。\"", 23});
        c2.push_back({"回到镇中心", 0});
        page2.choices = c2;
    } else if (playerClass == "法师") {
        page2.text = "小巷尽头一扇不起眼的木门。你推开——\n"
                     "里面的元力波动几乎让你退了半步。\n"
                     "不是人——是东西。货架上摆着至少三件被元力浸过的器物，\n"
                     "还有一堆看不懂文字的卷轴。\n\n"
                     "柜台后的瘦老头抬起头，金牙闪了一下。\n"
                     "\"你感觉到了是吧？\"他敲了敲柜台。\n"
                     "\"我这店里——每一件东西都有来历。\n"
                     "但你看的不是货。你在看——\"\n"
                     "他指了指墙角一堆旧书下面压着的羊皮纸。\n"
                     "\"那个。\"\n\n"
                     "\"那是青玉案八年前写的实验笔记。\n"
                     "我花了三年才弄到手——然后发现我看不懂。\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"笔记——我要了。\"", 21});
        c2.push_back({"\"（法师专属）你对龙骨了解多少？\"", 23});
        c2.push_back({"回到镇中心", 0});
        page2.choices = c2;
    } else if (playerClass == "富商") {
        page2.text = "小巷尽头一扇不起眼的木门。你推开——\n"
                     "里面就是个宝库。值钱的东西混在垃圾里，\n"
                     "但你的眼睛在一秒之内就挑出了三件能赚大钱的货。\n\n"
                     "柜台后的瘦老头抬起头，金牙闪了一下。\n"
                     "\"你的眼睛在转。你看到了货。\"\n"
                     "他往前探了探身。\n"
                     "\"我喜欢跟生意人打交道——不用讨价还价。\n"
                     "我有三样东西卖：情报、人命、和后悔药。\n"
                     "情报是真的，人命是别人的，后悔药——\"\n"
                     "他从柜台下拿出一个黑瓷瓶，晃了晃。\n"
                     "\"吃了之后你会回到上一个选择之前。\n"
                     "一生只能用一次。一千金币。\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"（富商专属）情报网——我要订阅。\"", 24});
        c2.push_back({"\"后悔药——太贵了。说个实价。\"", 21});
        c2.push_back({"回到镇中心", 0});
        page2.choices = c2;
    } else if (playerClass == "盗贼") {
        page2.text = "小巷尽头一扇不起眼的木门。你推开——\n"
                     "然后你笑了。这地方就是个贼窝。\n"
                     "值钱的东西随便摆，没锁，没陷阱——\n"
                     "只有一个小偷才会认出另一个小偷的自信。\n\n"
                     "柜台后的瘦老头抬起头，金牙闪了一下。\n"
                     "他也笑了。\n"
                     "\"同行。我一闻就知道。\"\n\n"
                     "他伸出一根枯干的手指，在柜台上敲了三下。\n"
                     "\"这店里的东西你可以看——但不许碰。\n"
                     "碰了算偷。偷同行的东西——在道上，比杀人更不体面。\"\n\n"
                     "他从柜台下拿出一串锈迹斑斑的钥匙。\n"
                     "\"不过这个——是送你的。玄铁城地牢钥匙。\n"
                     "整个南境只有三套。\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"（盗贼专属）同行——有什么好货？\"", 23});
        c2.push_back({"\"送我的？什么条件？\"", 21});
        c2.push_back({"回到镇中心", 0});
        page2.choices = c2;
    }
    pages.insert(2, page2);

    StoryPage page21;
    page21.text = "老鬼伸出一根干枯的手指，在柜台上敲了三下。\n\n"
                  "\"玄铁城里有个法师叫青玉案。过去三年他收购了大量龙骨——\n"
                  "每一块龙骨，都是一个死去的除咎师留下的元力结晶。\n"
                  "他买龙骨干什么？没人知道。\n"
                  "但有人在黑风洞最深处看到过一个被符文刻满全身的老头——\n"
                  "那是青玉案的师父。八年前被自己的徒弟亲手杀了。\"";
    page21.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(21, page21);

    StoryPage page22;
    page22.text = "老鬼笑了。金牙在烛火下闪闪发光。\n\n"
                  "\"杀谁？疤面？已经有人替你杀了。\n"
                  "铁峰？他不值得花钱——你自己去练兵场就能跟他打。\n"
                  "萧正阳？那得加钱。很多钱。\n"
                  "青玉案——\"\n\n"
                  "他收了笑。\n"
                  "\"这个不接。我虽然做黑市生意，但我不想死。\"";
    page22.choices.push_back({"回到镇中心", 0});
    pages.insert(22, page22);

    StoryPage page23;
    page23.text = "老鬼看到你的第一眼就笑了。\n"
                  "他笑起来没有声音——只有脸上的褶皱一层层堆起来。\n\n"
                  "\"同行。我一闻就知道。\"\n\n"
                  "他从柜台下拿出一串钥匙，每一把都锈迹斑斑——\n"
                  "但每一个锈斑都被人仔细地磨平了。\n\n"
                  "\"玄铁城地牢的备用钥匙。整个南境只有三套。\n"
                  "送给你。不是白送——\n"
                  "你要是能把这东西用在刀刃上，回来分我两成好处。\"";
    page23.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(23, page23);

    StoryPage page24;
    page24.text = "老鬼点了点头。\n\n"
                  "\"有钱人。我活了这么久，看一眼就知道谁的钱包是真的鼓。\n"
                  "这样——我手里有一张玄铁城的情报网。\n"
                  "遍布全城，每月一份最新消息。第一份免费。\"\n\n"
                  "他从柜子下拿出一张写满小字的纸。\n"
                  "\"这个月的消息：领主萧正阳的身体——不行了。\n"
                  "不是病。是有人在抽他的命。\"";
    page24.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(24, page24);

    // ---- 铁峰（按职业定制） ----
    StoryPage page3;
    if (playerClass == "战士") {
        page3.text = "练兵场上，一个魁梧军官正在操练新兵。\n"
                     "他看到你的时候，口令停了半拍——然后继续喊。\n"
                     "但你能感觉到，从那一刻起，他的注意力分了一半在你身上。\n\n"
                     "\"你——\"他操练结束，走到你面前，上下打量了一遍。\n"
                     "\"不是来投军的。投军的人不会站得这么直。\n"
                     "你是来找架打的。\"\n\n"
                     "他的手按上了腰间佩刀。不是威胁——是期待。\n"
                     "\"五年了。没有一个外人能在练兵场上站过一炷香。\n"
                     "你要试试吗？\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"（战士专属）切磋三招——点到为止。\"", 32});
        c3.push_back({"\"庄老三让我带句话：他还没死，还在打铁。\"", 34});
        c3.push_back({"\"我需要借兵——清剿黑风洞残余。\"", 31});
        c3.push_back({"回到镇中心", 0});
        page3.choices = c3;
    } else if (playerClass == "刺客") {
        page3.text = "练兵场上，一个魁梧军官正在操练新兵。\n"
                     "你站在围观的人群里，没有说话。\n\n"
                     "但他的目光扫过来的时候，在你身上停了一瞬。\n"
                     "不是发现了你——是一种老兵的直觉：\n"
                     "人群里有一个不一样的人。\n\n"
                     "操练结束后，他走到你面前。\n"
                     "\"你——刚才站在那个角度，刚好能看到西墙换岗的空档。\n"
                     "一般人不会注意那个位置。你是做什么的？\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"侦察。我需要了解玄铁城的防务。\"", 31});
        c3.push_back({"\"庄老三让我带句话。\"", 34});
        c3.push_back({"（沉默。让他自己判断。）", 33});
        c3.push_back({"回到镇中心", 0});
        page3.choices = c3;
    } else if (playerClass == "法师") {
        page3.text = "练兵场上，一个魁梧军官正在操练新兵。\n"
                     "你注意到他的右臂有一道发白的旧疤——边缘光滑，\n"
                     "不是刀剑伤。是元力灼烧。\n\n"
                     "他也注意到了你在看那道疤。操练完，他径直走过来。\n"
                     "\"你刚才在看我的伤。不是好奇——是认出来了。\"\n"
                     "他的声音很低，跟刚才操练时的嗓门判若两人。\n"
                     "\"三年前一个穿白袍的法师留下的。你认识他？\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"（法师专属）青玉案。他杀了我师父。\"", 33});
        c3.push_back({"\"我不是来找麻烦的——但我想了解情况。\"", 31});
        c3.push_back({"回到镇中心", 0});
        page3.choices = c3;
    } else if (playerClass == "富商") {
        page3.text = "练兵场上，一个魁梧军官正在操练新兵。\n"
                     "你站在场边，看了一会儿。然后他的目光落在了你身上。\n\n"
                     "\"你不是来投军的。\"他收了刀。\n"
                     "\"你穿的比镇长还好。你是来花钱的。\"\n\n"
                     "他走到你面前，甲片上的刀痕在日光下格外清晰。\n"
                     "\"说吧——你是来雇人的，还是来买兵器的？\n"
                     "先说好：人不出租，兵器不打折。\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"我要雇人。玄铁城那边需要帮手。\"", 31});
        c3.push_back({"\"庄老三的铸剑术——值不值得投资？\"", 34});
        c3.push_back({"回到镇中心", 0});
        page3.choices = c3;
    } else if (playerClass == "盗贼") {
        page3.text = "练兵场上，一个魁梧军官正在操练新兵。\n"
                     "你注意到他腰间的佩刀——刀镡松了。\n"
                     "不是质量问题，是反复拔刀之后自己缠的粗麻布松了。\n\n"
                     "他操练完，一眼就看到了你——\n"
                     "因为全场只有你一个人没有在看他，而是在看他的刀。\n\n"
                     "\"你——盯着我的刀看什么？\"\n"
                     "他拍了拍刀鞘。\"想偷？先过我这关。\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"刀镡松了。下次动手之前拧一下——不然刀身会飞出去。\"", 31});
        c3.push_back({"\"我不偷东西。但我可以帮你修好它。\"", 34});
        c3.push_back({"回到镇中心", 0});
        page3.choices = c3;
    }
    pages.insert(3, page3);

    StoryPage page31;
    page31.text = "铁峰用刀柄在地上画了一个简图。\n\n"
                  "\"青石镇四面有墙。但真正能守的——只有我和手下五十个兵。\n"
                  "山贼不来，是因为我在密林口子放了三个哨。\n"
                  "但这撑不了多久。\"\n\n"
                  "他收起刀。\n"
                  "\"你要是去玄铁城——帮我问问领主，\n"
                  "他答应给青石镇的援军，到底还来不来。\"";
    page31.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(31, page31);

    StoryPage page32;
    page32.text = "铁峰愣了一下。然后他仰天大笑——\n"
                  "那笑声震得练兵场上的沙子都在跳。\n\n"
                  "\"哈哈哈哈哈！好小子！五年没跟外人对练了！\"\n\n"
                  "他从兵器架上抽出一把木刀，扔给你一把。\n"
                  "木刀入手沉甸甸的——不是普通的训练刀，是铁桦木削的，\n"
                  "重量和真刀相差不到二两。\n\n"
                  "他活动了一下肩膀，骨节发出一串脆响。\n"
                  "\"三招。你要能逼我后退一步，算你赢。\n"
                  "赢了——我腰上这把真刀给你。输了——今晚请我喝酒。\n"
                  "这买卖公道不公道？\"\n\n"
                  "他摆了一个起手式。木刀横在身前，刀尖微微下垂——\n"
                  "这不是攻击的姿态，是防守的姿态。\n"
                  "他在等你先手。这是老兵的自信。";
    page32.choices.push_back({"——出招——", 321});
    pages.insert(32, page32);

    StoryPage page321;
    page321.text = "第一招——你出的是一记直劈。\n\n"
                    "没有虚晃，没有试探，就是最简单、最直接的正面劈砍。\n"
                    "这是战士之间的默契——第一招不玩花活，先试试对方的斤两。\n\n"
                    "两把木刀在空中碰撞。\"嘭\"的一声——\n"
                    "不是金属碰撞的脆响，而是木头发出的沉闷撞击声，\n"
                    "像两棵大树在风中撞在了一起。\n\n"
                    "你的虎口一震。铁峰的臂力比你预想的要大——\n"
                    "这个在练兵场上站了十年的人，骨头里都渗进了铁。\n"
                    "但你的刀没有偏。\n\n"
                    "铁峰的眉毛挑了一下。\n"
                    "\"有劲。再来。\"\n\n"
                    "第二招——他主动出手了。\n"
                    "木刀从他腰间横斩而来——角度刁钻，\n"
                    "不像第一招那样正大光明，而是带着沙场上的实战经验。\n"
                    "那些新兵在训练场上套的招，跟这一刀比起来，就是儿戏。\n\n"
                    "你竖起木刀格挡。木刀再次碰撞——\n"
                    "这一次的力道比第一次重了一倍。\n"
                    "你的脚在沙地上滑了半寸。但你的身体没有退。\n\n"
                    "铁峰的眼睛亮了。\n"
                    "\"不退？有意思。\"\n\n"
                    "第三招——\n"
                    "他举刀过顶。这是一个大开大合的劈砍姿态。\n"
                    "但你在谛观状态下看到了一样东西——\n"
                    "他的前脚重心偏了。不是失误——是故意的。\n"
                    "他在卖破绽。\n\n"
                    "他在试你。\n"
                    "试你是会抓住这个破绽趁势进攻——\n"
                    "还是会点到为止，给他留一个老兵的面子。";
    page321.choices.push_back({"点到为止——收刀", 3211});
    page321.choices.push_back({"趁势进攻——破他的重心", 3212});
    pages.insert(321, page321);

    StoryPage page3211;
    page3211.text = "你的木刀在离他肩膀三寸的地方停了。\n\n"
                     "不是劈不到——是你不劈。\n\n"
                     "铁峰的木刀也停在了半空中。\n"
                     "他保持着那个故意卖出的破绽姿势，愣了整整两息。\n\n"
                     "然后他收了刀。\n"
                     "不是把刀放回兵器架——是收起了那个老兵的气场。\n\n"
                     "\"好。\"\n"
                     "他说这个字的时候没有笑。\n"
                     "但比笑更真挚。\n\n"
                     "\"五年了——每一个跟我切磋的人都想赢我。\n"
                     "你是第一个看到破绽却不利用的。\"\n\n"
                     "他从腰间解下佩刀——那把真正的佩刀，\n"
                     "刀鞘上缠着防滑的粗麻布。\n"
                     "\"这把刀跟了我十年。拿去。\n"
                     "不是因为你赢了——\n"
                     "是因为你让我想起来，仗不只是打赢才算赢。\"";
    page3211.choices.push_back({"收下佩刀，前往玄铁城", -2, "city"});
    pages.insert(3211, page3211);

    StoryPage page3212;
    page3212.text = "你没有犹豫。\n\n"
                     "木刀顺着铁峰露出的破绽切入——\n"
                     "不是劈他的肩膀，而是用刀背敲在他的前腿上。\n"
                     "他的重心本来就在那条腿上——\n"
                     "你敲的位置刚好让他的膝盖本能地弯了一下。\n\n"
                     "铁峰后退了一步。\n"
                     "只有一步。但对于一个在沙场上站了十年没退过的老兵来说——\n"
                     "这一步，就是承认。\n\n"
                     "他稳住身形，看着你，沉默了三息。\n"
                     "然后他大笑——笑得比之前任何一次都响，\n"
                     "震得兵器架上的刀剑都在微微颤抖。\n\n"
                     "\"好！痛快！比我带的所有兵都痛快！\"\n\n"
                     "他从腰间解下佩刀，抛给你。\n"
                     "\"这把刀跟了我十年。现在归你了。\n"
                     "记住——刀可以输，人不可以。\"";
    page3212.choices.push_back({"收下佩刀，前往玄铁城", -2, "city"});
    pages.insert(3212, page3212);

    StoryPage page33;
    page33.text = "铁峰的笑容凝固了。\n\n"
                  "他低头看了看自己手臂上一道发白的旧疤——\n"
                  "边缘光滑，不是刀剑造成的。\n\n"
                  "\"三年前。一个穿白袍的法师路过青石镇。\n"
                  "他的马踩翻了一个路边摊，摊主骂了一句——\n"
                  "那个法师抬手就杀了摊主。\"\n\n"
                  "\"我带两个兵上去。他连手指都没动——\n"
                  "光用元力往外一震，我们就飞出去三丈远。\n"
                  "那两个兵没活下来。\"\n\n"
                  "他握紧了刀柄。指节发白。\n"
                  "\"你要是冲那个法师去的——我铁峰欠你一个人情。\"";
    page33.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(33, page33);

    StoryPage page34;
    page34.text = "铁峰拿刀的手顿了一下。然后他收了刀。\n\n"
                  "\"庄老三……\"\n\n"
                  "他望向桃源村的方向，沉默了很久。\n\n"
                  "\"三十七个人，只剩我们两个。\n"
                  "他选择了打铁，我选择了练兵。\n"
                  "谁也没比谁好过。\"\n\n"
                  "他转过头，眼睛里有一种你之前没看到的坚定。\n"
                  "\"替我回他一句——铁峰也没死。还在守城门。\"";
    page34.choices.push_back({"（记下这句话）", 0});
    pages.insert(34, page34);

    // ---- 血狼 ----
    StoryPage page4;
    page4.text = "酒馆角落里，一个满脸横肉的壮汉正用匕首剔牙。\n"
                 "桌上摆着三个空酒坛和一把巨大的双手斧，斧刃上有道很深的豁口。\n"
                 "周围三张桌子空空荡荡——没人敢靠近。\n\n"
                 "\"雇人？杀人？还是雇人杀人？\"\n\n"
                 "\"先说好——我血狼的价码不是最便宜的。\n"
                 "但你出去打听打听，这地界我办不了的事，别人也办不了。\"";
    QVector<StoryChoice> c4;
    c4.push_back({"\"我要雇你杀人。\"", 41});
    c4.push_back({"\"我要雇你打仗。\"", 42});
    if (playerClass == "富商") {
        c4.push_back({"\"（富商专属）打完玄铁城，我给你一份长期差事。\"", 43});
    }
    c4.push_back({"回到镇中心", 0});
    page4.choices = c4;
    pages.insert(4, page4);

    StoryPage page41;
    page41.text = "\"杀谁？\"血狼把匕首钉在桌上。\n\n"
                  "\"疤面？他已经死了。赤眉？他是叛军头子，不好杀——\n"
                  "得加钱。萧正阳？他是领主，杀他等于造反——\n"
                  "得加很多钱。\"\n\n"
                  "\"青玉案？\"他咧嘴，露出一口被酒染黄的牙。\n"
                  "\"这个不要钱——我也讨厌法师。\"";
    page41.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(41, page41);

    StoryPage page42;
    page42.text = "血狼把斧子扛上肩膀。\n\n"
                  "\"打仗？好。我最擅长的就是打仗。\n"
                  "玄铁城我熟——城门有几道锁，城墙有几处弱点，\n"
                  "禁卫军的换岗时间——我都摸清了。\n"
                  "五百金币，我和我手下二十个兄弟跟你走。\"\n\n"
                  "他顿了顿。\n"
                  "\"先付一半。死了不退。\"";
    page42.choices.push_back({"前往玄铁城", -2, "city"});
    pages.insert(42, page42);

    StoryPage page43;
    page43.text = "血狼把嵌在桌里的匕首拔了出来，咧嘴一笑。\n\n"
                  "\"老板——所有价码给你打对折。\n"
                  "只有一个条件。\"\n\n"
                  "他凑近了一点，你能闻到他身上的酒气和铁锈味。\n"
                  "\"打完了玄铁城，我要在你手下做事。\n"
                  "当个看门的也好，当个随从也好。管饭就行。\"\n\n"
                  "他沉默了一拍，又补了一句——声音忽然轻了。\n"
                  "\"我打了一辈子仗。不想再换东家了。\"";
    page43.choices.push_back({"成交。前往玄铁城", -2, "city"});
    pages.insert(43, page43);

    screenControl->setStories(pages);
}

void town::resetStory() { screenControl->resetStory(); }
void town::onReturnToParent() { emit Return(); screenTransition->hideScreen(this); }
void town::onSceneActionRequested(const QString& action) { screenTransition->hideScreen(this); emit navigateTo(action); }
void town::on_button_back_clicked(bool) { emit Return(); screenTransition->hideScreen(this); }
void town::on_button_next_clicked(bool) { travel *d = new travel(this); connect(d, &travel::destinationSelected, this, &town::navigateTo); connect(d, &travel::destinationSelected, d, &QDialog::deleteLater); d->exec(); }
void town::on_button3_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) { track("mayor_choice",11); ui->button_next->setVisible(true); } if(c==2) { track("heishi_choice",21); ui->button_next->setVisible(true); } if(c==3) { track("tiefeng_choice",1); ui->button_next->setVisible(true); } if(c==4) { track("xuelang_choice",41); ui->button_next->setVisible(true); } screenControl->onChoiceClicked(0); if(screenControl->getCurrentPage()==0) refreshHub(); }
void town::on_button4_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("mayor_choice",12); if(c==2) track("heishi_choice",22); if(c==3) track("tiefeng_choice",2); if(c==4) track("xuelang_choice",42); screenControl->onChoiceClicked(1); if(screenControl->getCurrentPage()==0) refreshHub(); }
void town::on_button5_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("mayor_choice",13); if(c==2) track("heishi_choice",23); if(c==4) track("xuelang_choice",43); screenControl->onChoiceClicked(2); if(screenControl->getCurrentPage()==0) refreshHub(); }
void town::on_button6_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("mayor_choice",14); if(c==2) track("heishi_choice",24); screenControl->onChoiceClicked(3); if(screenControl->getCurrentPage()==0) refreshHub(); }
void town::on_button7_clicked(bool) { screenControl->onChoiceClicked(4); }
void town::track(const QString& k, const QVariant& v) { m_state[k]=v; }
QVariant town::st(const QString& k) const { return m_state.value(k); }
void town::refreshHub() { if(!pages.contains(0)) return; StoryPage& p=pages[0]; p.choices.clear(); p.choices.push_back({"前往镇长府衙（沈大人）", 1}); p.choices.push_back({"前往黑市小巷（老鬼）", 2}); p.choices.push_back({"前往练兵场（铁峰）", 3}); p.choices.push_back({"前往酒馆（血狼）", 4}); if(st("mayor_choice").toInt()==11||st("mayor_choice").toInt()==13) p.choices.push_back({"（有镇长推荐信）前往玄铁城", -2, "city"}); if(st("heishi_choice").toInt()==21||st("heishi_choice").toInt()==23) p.choices.push_back({"（获得情报）前往玄铁城", -2, "city"}); if(st("tiefeng_choice").toInt()>0) p.choices.push_back({"（铁峰的人情）前往玄铁城", -2, "city"}); if(st("xuelang_choice").toInt()==43) p.choices.push_back({"（雇佣血狼）前往玄铁城", -2, "city"}); p.choices.push_back({"前往玄铁城", -2, "city"}); p.choices.push_back({"返回黑风洞", -2, "cave"}); }
