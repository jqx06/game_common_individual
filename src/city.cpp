#include "city.h"
#include "ui_city.h"
#include "travel.h"
#include "musicmanager.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

city::city(QWidget *parent)
    : QWidget(parent), ui(new Ui::city),
      screenTransition(new ScreenTransition(this)),
      screenControl(new ScreenControl(this)), playerClass("")
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/city_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "city");

    screenControl->setupTextEdit(ui->textEdit);
    QGraphicsDropShadowEffect *ts = new QGraphicsDropShadowEffect();
    ts->setBlurRadius(6);
    ts->setOffset(3, 3);
    ts->setColor(QColor(0, 0, 0, 180));
    ui->textEdit->setGraphicsEffect(ts);
    screenControl->setButtons(ui->button3, ui->button4, ui->button5, ui->button6, ui->button7);
    ui->button_next->setVisible(false);
    ui->button_next->setText("移动");
    connect(screenControl, &ScreenControl::returnToParent, this, &city::onReturnToParent);
    connect(screenControl, &ScreenControl::sceneActionRequested, this, &city::onSceneActionRequested);
    rebuildStory();
    screenControl->resetStory();
}

city::~city() { delete ui; }

void city::setPlayerClass(const QString& className) { playerClass = className; rebuildStory(); screenControl->resetStory(); }

void city::rebuildStory()
{
    pages.clear();

    StoryPage page0;
    page0.text = "玄铁城——南境的心脏。\n\n"
                 "黑铁铸造的城墙高逾十丈，在夕阳下泛着暗红色的光。\n"
                 "城门口排列着两行重甲士兵，盔甲上的铁刺密密麻麻，\n"
                 "让人分不清这是城门还是猛兽的牙口。\n\n"
                 "城门前立着一块石碑，碑上的字被夕阳染成暗金色：\n"
                 "「进城者卸甲。卸不下甲，就卸下命。」\n\n"
                 "你站在这扇门前。在你的前方——\n"
                 "玄铁殿、公主庭院、叛军地下室、以及最高的那座露台。";
    if (playerClass == "战士") {
        page0.text += "\n\n玄铁殿的大门敞开着。你能感觉到——铁椅上的那个人在等你。";
        page0.choices.push_back({"★ 前往玄铁殿——直面领主萧正阳", 1});
        page0.choices.push_back({"前往公主庭院（卿伊瑟）", 2});
        page0.choices.push_back({"寻找叛军（赤眉）", 3});
    } else if (playerClass == "刺客") {
        page0.text += "\n\n你注意到了城墙下的暗哨。顺着阴影摸过去——叛军的地下室就在不远处。";
        page0.choices.push_back({"★ 潜入地下——寻找叛军赤眉", 3});
        page0.choices.push_back({"前往玄铁殿（领主·萧正阳）", 1});
        page0.choices.push_back({"前往公主庭院（卿伊瑟）", 2});
    } else if (playerClass == "法师") {
        page0.text += "\n\n整座城中，最高的那座露台上散发着浓烈的龙骨气息。青玉案就在那里。";
        page0.choices.push_back({"★ 登上高台——直面青玉案", 4});
        page0.choices.push_back({"前往玄铁殿（领主·萧正阳）", 1});
        page0.choices.push_back({"前往公主庭院（卿伊瑟）", 2});
    } else if (playerClass == "富商") {
        page0.text += "\n\n这座城就是最大的生意场。从领主到叛军——每个人都有自己的价码。";
        page0.choices.push_back({"前往玄铁殿（领主·萧正阳）", 1});
        page0.choices.push_back({"前往公主庭院（卿伊瑟）", 2});
        page0.choices.push_back({"寻找叛军（赤眉）", 3});
        page0.choices.push_back({"登上高台（青玉案）", 4});
    } else if (playerClass == "盗贼") {
        page0.text += "\n\n你摸了摸怀里老鬼给的地牢钥匙。公主的庭院——守卫最松的那一侧——\n从鱼池翻过去，不会惊动任何人。";
        page0.choices.push_back({"★ 潜入公主庭院——见卿伊瑟", 2});
        page0.choices.push_back({"前往玄铁殿（领主·萧正阳）", 1});
        page0.choices.push_back({"寻找叛军（赤眉）", 3});
    }
    page0.choices.push_back({"返回青石镇", -2, "town"});
    pages.insert(0, page0);

    // ---- 萧正阳（按职业定制） ----
    StoryPage page1;
    if (playerClass == "战士") {
        page1.text = "玄铁殿——从殿门到领主铁椅，一百步。\n"
                     "你每走一步，铁砖上的花纹就在脚下微微发亮。\n\n"
                     "萧正阳坐在那张铁椅上。五十岁上下，面容棱角分明。\n"
                     "一身玄色长袍，手里什么都没有——\n"
                     "但元力压迫像一座山一样压在你肩上。\n\n"
                     "你没有坐下。你站在原地，抬头直视他。\n\n"
                     "\"你不坐。\"他说。不是疑问。\n"
                     "\"来我这里的人，有的跪，有的坐，有的发抖——\n"
                     "你是第一个站着的。\"\n\n"
                     "他从铁椅上缓缓起身。\n"
                     "\"好。站着说话的人——值得我站起来听。\n"
                     "说吧。你从桃源村一路走到这里——为了什么？\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"瑶儿已救出。山贼已除。下一步是你的城。\"", 11});
        c1.push_back({"\"（战士专属）天下不是一个人守得住的。\"", 14});
        c1.push_back({"回到城门口", 0});
        page1.choices = c1;
    } else if (playerClass == "刺客") {
        page1.text = "玄铁殿——一百步。每一步都踩在铁砖的花纹上。\n"
                     "你数了大殿里有多少个禁卫军。十二个。换岗间隔——\n"
                     "大约六十息。从大门到铁椅背后——十一步。\n\n"
                     "萧正阳坐在铁椅上。他的声音不大，但每个字都清楚：\n"
                     "\"你进来之后看的第一眼——不是我的脸，\n"
                     "是我身后的暗门。\"\n\n"
                     "他顿了顿。\n"
                     "\"能一眼找到这个大殿唯一弱点的人——\n"
                     "不是普通的访客。你是谁的人？\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"我谁的人都不是。我是来找真相的。\"", 11});
        c1.push_back({"（沉默。手在袖子里握住了匕首柄。）", 14});
        c1.push_back({"回到城门口", 0});
        page1.choices = c1;
    } else if (playerClass == "法师") {
        page1.text = "玄铁殿——从殿门到铁椅，一百步。\n"
                     "每走一步，你感受到的元力压迫就重一分——\n"
                     "但这不是萧正阳自己的力量。\n"
                     "是他的脊柱里——被植入的龙骨在替他撑着。\n\n"
                     "萧正阳坐在铁椅上。他的脸色比你想象中更差。\n"
                     "\"你的眼神跟别人不一样。别人看我的脸——\n"
                     "你在看我的后背。\"\n\n"
                     "他微微前倾。\n"
                     "\"你就是黑风洞那个老东西说的——\n"
                     "迟早会来的人？\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"（法师专属）你体内的龙骨——青玉案多久抽一次？\"", 13});
        c1.push_back({"\"我不是来杀你的。我是来终结这一切的。\"", 12});
        c1.push_back({"回到城门口", 0});
        page1.choices = c1;
    } else if (playerClass == "富商") {
        page1.text = "玄铁殿——一百步。铁砖花纹下藏着暗槽——\n"
                     "这大殿的地暖系统造价至少三万金币。\n"
                     "萧正阳是个舍得花钱的领主。这对你来说——是好消息。\n\n"
                     "萧正阳坐在铁椅上。他打量了你一眼——\n"
                     "然后嘴角微微扬起。\n"
                     "\"你不是来打仗的。你穿的比打仗的人好太多。\n"
                     "你是来谈生意的。\"\n\n"
                     "他做了一个'请坐'的手势。\n"
                     "\"坐。我三年没见过生意人了。\n"
                     "封锁城关之后——商路断了。所以你走密林来的？\n"
                     "那条路能走通——就值一条商路。\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"我要的比商路大——我要整个南境的经营权。\"", 11});
        c1.push_back({"\"我先帮你解决青玉案。然后我们再谈生意。\"", 12});
        c1.push_back({"回到城门口", 0});
        page1.choices = c1;
    } else if (playerClass == "盗贼") {
        page1.text = "玄铁殿——从殿门到铁椅，一百步。\n"
                     "你注意到铁椅下的石板纹路不自然——\n"
                     "那是一个翻板机关。开关在萧正阳右手扶手下三寸。\n"
                     "有人在监视这张椅子。\n\n"
                     "萧正阳坐在铁椅上。他看着你。\n"
                     "\"你的眼睛在动。不是害怕——是在算。\n"
                     "你在算大殿里有多少人？出口在哪里？\n"
                     "还是——\"他顿了顿，\"你在算怎么偷我的禁军调令？\"";
        QVector<StoryChoice> c1;
        c1.push_back({"\"我没打算偷——但你的铁椅下有机关。你知道吗？\"", 11});
        c1.push_back({"\"我需要进地牢。听说只有你的令牌能开。\"", 12});
        c1.push_back({"回到城门口", 0});
        page1.choices = c1;
    }
    pages.insert(1, page1);

    StoryPage page11;
    page11.text = "萧正阳沉默了很久。然后他从铁椅上缓缓站起。\n\n"
                  "\"瑶儿是卿伊瑟的贴身丫鬟。你能把她救出来——\n"
                  "我欠你一个人情。\"\n\n"
                  "他走到你面前。近看之下，他的脸色比你想象的还要差。\n"
                  "\"你想要什么？官职？封地？还是钱？\"";
    page11.choices.push_back({"\"我要真相。青玉案对你做了什么？\"", 12});
    page11.choices.push_back({"\"什么都不要。我只是做了该做的事。\"", 111});
    pages.insert(11, page11);

    StoryPage page111;
    page111.text = "萧正阳盯着你看了很久。那双被元力榨干了神采的眼睛里，\n"
                   "忽然亮了一下。\n\n"
                   "\"什么都不要……好。好。\"\n\n"
                   "他转过身，背对着你。\n"
                   "\"三年前青玉案找到我。他说他可以让我变得足够强大——\n"
                   "强大到能守住这座城。代价是我的生命会变成他仪式的燃料。\n"
                   "我答应了。\"\n\n"
                   "\"我以为我能撑到安排好一切。\n"
                   "但我快撑不住了。\"\n\n"
                   "他转过身，那双深陷的眼睛里，\n"
                   "你看到了一个父亲、一个领主的最后请求。\n"
                   "\"替我保护好卿伊瑟。她是这座城——唯一的希望。\"";
    page111.choices.push_back({"去找卿伊瑟", 2});
    page111.choices.push_back({"去高台找青玉案", 4});
    pages.insert(111, page111);

    StoryPage page12;
    page12.text = "萧正阳的瞳孔骤然收缩。\n\n"
                  "\"你知道青玉案的事？\"\n\n"
                  "他缓缓坐回铁椅。你注意到他坐下的时候，\n"
                  "后背死死贴着椅背——像是在从椅背上汲取着什么。\n\n"
                  "\"三年了。你是第一个敢在我面前提这件事的人。\n"
                  "也是第一个——让我觉得也许还来得及的人。\"\n\n"
                  "他从铁椅扶手下取出一样东西——一枚令牌。\n"
                  "\"这是禁军调令。凭此可调动玄铁城三分之一兵力。\n"
                  "但我有一个条件——\"\n"
                  "\"不管发生什么，别伤害卿伊瑟。\"";
    page12.choices.push_back({"去找青玉案对质", 4});
    page12.choices.push_back({"去找卿伊瑟", 2});
    pages.insert(12, page12);

    StoryPage page13;
    page13.text = "萧正阳的身体僵住了。大殿里的空气在那一瞬间凝住了。\n\n"
                  "他盯着你看了很久。然后他笑了——\n"
                  "不是胜利的笑，是被人戳穿了之后松了一口气的苦笑。\n\n"
                  "\"你说对了。三年了——你是第一个看出来的人。\"\n\n"
                  "他转过身，背对着你。隔着玄色长袍，\n"
                  "你感受到了——那种元力涌动的痕迹，密密麻麻，\n"
                  "像无数条锁链从他的脊柱蔓延开来。\n\n"
                  "\"青玉案把龙骨植入我的脊柱。\n"
                  "每过一天，他抽取我一年的寿命。\n"
                  "我以为我能撑到把一切都安排好——\n"
                  "但现在，我已经没有多少时间了。\"";
    page13.choices.push_back({"去高台——终结这一切", 4});
    pages.insert(13, page13);

    StoryPage page14;
    page14.text = "萧正阳从铁椅上站起来。这是他第一次正眼看你。\n\n"
                  "\"天下不是一个人守得住的——你说得对。\n"
                  "我用了二十年才明白这个道理。\"\n\n"
                  "他从腰间解下佩剑——那把刻着「正」字的剑。\n"
                  "\"这把剑我佩了二十年。现在我不配用了。\n"
                  "拿着——或者不拿。随你。\"\n\n"
                  "他把剑放在你面前的台阶上。\n"
                  "然后他转身，一步一步走向玄铁殿的深处。\n"
                  "每一步都踩得很稳——比之前任何一步都稳。";
    page14.choices.push_back({"去高台找青玉案", 4});
    pages.insert(14, page14);

    // ---- 卿伊瑟 ----
    StoryPage page2;
    page2.text = "你不是在大殿见到她的。\n\n"
                 "你是在城中一处僻静的庭院——一方鱼池旁，\n"
                 "一个少女背对你站着，手里捏着一把鱼食，\n"
                 "正一颗一颗地往池中投。\n\n"
                 "她穿着一身素白长裙，头发简单地束在脑后。\n"
                 "听到脚步声，她没有回头。只是停了投鱼食的手。\n\n"
                 "\"瑶儿是你救的。\"\n"
                 "不是疑问句。是陈述句。\n"
                 "\"我欠你一句谢谢。\"\n\n"
                 "她转过身。十六七岁的年纪，\n"
                 "却有一双不属于这个年纪的眼睛——太安静了。\n"
                 "\"但我也要问你——欠你的这个人情，你想怎么还？\"";
    page2.choices.push_back({"\"不用还。救人是我的选择。\"", 21});
    page2.choices.push_back({"\"我要知道青玉案的事。\"", 22});
    page2.choices.push_back({"\"你父亲——他快撑不住了。\"", 23});
    page2.choices.push_back({"\"你信我吗？\"", 24});
    page2.choices.push_back({"回到城门口", 0});
    pages.insert(2, page2);

    StoryPage page21;
    page21.text = "卿伊瑟沉默了片刻，将手里最后一颗鱼食投入池中。\n"
                   "锦鲤争抢，水面泛起一圈小小的涟漪。\n\n"
                   "\"不用还。你说的。\"\n\n"
                   "她转过身去，重新背对你。\n"
                   "\"但我记性好。欠人的东西，迟早会还——\n"
                   "不管你要不要。\"\n\n"
                   "你没有获得即时回报。\n"
                   "但在你转身离开时，你听到她在你身后说了一句话。\n"
                   "\"高台上的那个人——他不吃棋子。\n"
                   "他吃下棋的人。\"";
    page21.choices.push_back({"去高台", 4});
    pages.insert(21, page21);

    StoryPage page22;
    page22.text = "卿伊瑟的手终于放下了。\n\n"
                   "\"青玉案。他救过我父亲的命——三年前。\n"
                   "所有人都说他是个好人。\"\n\n"
                   "她顿了顿。鱼池里的锦鲤开始散开了，像是感知到了什么。\n\n"
                   "\"但我不信。你知道为什么吗？\n"
                   "因为从那年起，我再也没有进过我父亲的房间。\n"
                   "以前我每天都能进。\"\n\n"
                   "她的声音依然平静，但最后一个字的尾音，\n"
                   "有一丝极细微的颤抖。";
    page22.choices.push_back({"去高台找青玉案", 4});
    pages.insert(22, page22);

    StoryPage page23;
    page23.text = "卿伊瑟的身体震了一下。很轻。像是鱼池里落了一片叶子。\n\n"
                   "\"我知道。\"\n\n"
                   "她转过身。你第一次看到她的眼眶里有东西在打转——\n"
                   "但没有落下来。\n\n"
                   "\"我什么都知道。龙骨的秘密。青玉案的仪式。\n"
                   "我父亲在用他的命换这座城的安全。\n"
                   "然后青玉案在偷我父亲的命去打开一扇不该打开的门。\"\n\n"
                   "\"我什么都知道。却什么都做不了。\"";
    page23.choices.push_back({"\"现在你做得了。跟我来。\"", 4});
    pages.insert(23, page23);

    StoryPage page24;
    page24.text = "\"你信我吗？\"\n\n"
                   "卿伊瑟没有立刻回答。她静静地看着你，像是在辨认什么——\n"
                   "不是辨认你的脸，而是辨认更深层的东西。\n\n"
                   "三秒。然后她将手里整把鱼食全部洒进了池中。\n"
                   "锦鲤疯了一样地争抢，水面被搅得哗哗作响。\n\n"
                   "\"你说。\"\n"
                   "只有两个字。但两个字就够了。\n\n"
                   "她交给你的——是一枚禁军调令。\n"
                   "可以调动玄铁城三分之一的兵力。\n"
                   "\"子时之前有效。过了子时——就没有玄铁城了。\"";
    page24.choices.push_back({"去高台——终结这一切", 4});
    pages.insert(24, page24);

    // ---- 赤眉 ----
    StoryPage page3;
    page3.text = "外城一间废弃仓库的地下室。\n"
                 "烛火在潮湿的空气中摇曳，一张南境地图钉在墙上。\n\n"
                 "一个额头纹着两道暗红色纹路的年轻男人正站在地图前，\n"
                 "双臂布满烧伤疤痕——那不是普通的火烧，是元力之火。\n\n"
                 "他身后站着二十多个眼神疲惫但还没有熄灭的人。\n\n"
                 "\"我叫赤眉。\"他指着自己额头上的红疤。\n"
                 "\"三年前那场火把我的眉毛烧没了，长不回来了。\n"
                 "所以干脆就叫赤眉。\"\n\n"
                 "\"萧正阳欠我们很多条命。\n"
                 "你是来帮我们的——还是来抓我们的？\"";
    QVector<StoryChoice> c3;
    c3.push_back({"\"我想知道真相。为什么反？\"", 31});
    c3.push_back({"\"我可以帮你。但我有条件。\"", 32});
    if (playerClass == "战士") {
        c3.push_back({"\"（战士专属）你们缺的不是理由。你们缺的是能打的人。\"", 33});
    }
    if (playerClass == "富商") {
        c3.push_back({"\"（富商专属）我出钱，你出人。怎么样？\"", 34});
    }
    c3.push_back({"回到城门口", 0});
    page3.choices = c3;
    pages.insert(3, page3);

    StoryPage page31;
    page31.text = "赤眉把炭笔拍在地图上，在玄铁城的位置留下一个黑手印。\n\n"
                  "\"萧正阳修这座铁城的时候，征了三千民夫。\n"
                  "三千人修了三年——进去的时候是三千，出来的不到一千。\"\n\n"
                  "\"我哥就是那两千人之一。死在城墙下面。\n"
                  "连名字都没留下。\"\n\n"
                  "他收起愤怒，重新变回那个冷静的叛军首领。\n"
                  "\"我们要的不是复仇。我们要一个公道。\"";
    page31.choices.push_back({"\"我帮你要这个公道。\"", 4});
    pages.insert(31, page31);

    StoryPage page32;
    page32.text = "\"条件？\"赤眉挑起眉毛。两条红疤在烛火下像在燃烧。\n\n"
                  "\"说。\"";
    page32.choices.push_back({"\"战后——玄铁城的粮仓归青石镇。\"", 4});
    page32.choices.push_back({"\"战后——不许碰卿伊瑟。\"", 4});
    pages.insert(32, page32);

    StoryPage page33;
    page33.text = "赤眉看着你，然后伸出一只布满烧伤疤痕的手。\n\n"
                  "\"好。叛军缺的就是能打的人。\"\n\n"
                  "你们握了手。他的手很粗糙，但握力很大。\n"
                  "\"打完这一仗——我请你喝酒。\"";
    page33.choices.push_back({"前往高台", 4});
    pages.insert(33, page33);

    StoryPage page34;
    page34.text = "赤眉一愣。然后他仰头笑了——\n"
                  "笑声响亮，在地下室里回荡了好几圈。\n\n"
                  "\"有钱人！我们这些人穷了一辈子，\n"
                  "还没见过一个愿意给叛军投钱的老板。\"\n\n"
                  "他收起笑，认真地伸出一只手。\n"
                  "\"成交。你出钱，我们出命。战后成功了——你要什么？\"";
    page34.choices.push_back({"\"商路——青石镇到玄铁城的经营权。\"", 4});
    page34.choices.push_back({"\"一个人——卿伊瑟，你们不许碰。\"", 4});
    pages.insert(34, page34);

    // ---- 青玉案（按职业定制） ----
    StoryPage page4;
    if (playerClass == "战士") {
        page4.text = "城中最高处，四面无墙的高台。夜风穿堂而过。\n"
                     "一个穿着月白汉服的中年男人坐在石凳上，面前摆着一局残棋。\n\n"
                     "\"你来了。\"他没有抬头。\n"
                     "\"从桃源村到黑风洞，从青石镇到玄铁城——\n"
                     "你的每一步我都算到了。只有一步我没算到——\"\n\n"
                     "他终于抬起头。\n"
                     "\"你没死在疤面的刀下。不但没死——你还让他服了。\n"
                     "这世上能让疤面服气的人不超过三个。\n"
                     "一个是铁峰。一个是萧正阳。第三个——\"\n"
                     "他指了指你。\n"
                     "\"有意思。一个匹夫的拳头——\n"
                     "比我四十七块龙骨还管用？\"";
        QVector<StoryChoice> c4;
        c4.push_back({"\"拳头比龙骨诚实。它不说谎。\"", 41});
        c4.push_back({"\"掀了你的棋盘——我们换个方式解决。\"", 43});
        c4.push_back({"回到城门口", 0});
        page4.choices = c4;
    } else if (playerClass == "刺客") {
        page4.text = "城中最高处，四面无墙的高台。\n"
                     "一个穿月白汉服的中年男人面对残局——\n"
                     "但他没有坐下。他站着。因为他感觉到了。\n\n"
                     "\"出来。\"他头也不回。\n"
                     "\"你在高台下面等了多久？一炷香？一炷半？\n"
                     "你在等我落子——等我注意力全在棋盘上的那个瞬间。\"\n\n"
                     "他缓缓转过身。\n"
                     "\"但你知道吗——刺客最大的弱点不是手慢。\n"
                     "是太依赖时机。你在等时机——而我在等你。\"\n\n"
                     "他手中那枚白子，始终没有落下。";
        QVector<StoryChoice> c4;
        c4.push_back({"\"那你算到这一步了吗？\"（已在他身后）", 41});
        c4.push_back({"\"我不是来杀你的。我是来问一个人。\"", 44});
        c4.push_back({"回到城门口", 0});
        page4.choices = c4;
    } else if (playerClass == "法师") {
        page4.text = "城中最高处，四面无墙的高台。\n"
                     "你踏上最后一级台阶时——元力波动让你的每一根触角都在震颤。\n\n"
                     "青玉案坐在残局前。月白汉服在夜风中纹丝不动。\n"
                     "四十七块龙骨在他体内同时亮起——\n"
                     "每一块都是一条命，每一条命都在发出幽蓝色的哀鸣。\n\n"
                     "\"我知道你会来。\"他没有抬头。\n"
                     "\"从黑风洞那个老东西告诉你龙骨的时候——\n"
                     "你就在来的路上了。\"\n\n"
                     "他终于抬头。眼眶里——不是眼珠，是幽蓝色的光。\n"
                     "和黑风洞里那个老者临死前一模一样。\n\n"
                     "\"你跪了吗？跪给他——还是选择了站着？\"";
        QVector<StoryChoice> c4;
        c4.push_back({"\"（法师专属）你的龙骨仪式——该终止了。\"", 42});
        c4.push_back({"\"我跪了。不是因为怕——是因为他值得。\"", 41});
        c4.push_back({"回到城门口", 0});
        page4.choices = c4;
    } else if (playerClass == "富商") {
        page4.text = "城中最高处，四面无墙的高台。\n"
                     "青玉案坐在石凳上，面前摆着一局残棋。\n\n"
                     "你看了棋局三秒——然后看了高台的结构。\n"
                     "这地方造价不菲。四面无墙但风雨不侵——\n"
                     "是有元力结界在撑着。奢侈。\n\n"
                     "\"你不看棋。\"青玉案终于抬头。\n"
                     "\"你在算这座高台值多少钱。\"\n\n"
                     "他笑了。那种笑不是轻蔑——是遇到了同类的认可。\n"
                     "\"你跟他们都不一样。他们一上来就谈正义、谈天道、谈使命。\n"
                     "你——在算账。很好。\"\n\n"
                     "\"那就用你最擅长的方式：\n"
                     "开个价。你要什么——才肯不干涉我的计划？\"";
        QVector<StoryChoice> c4;
        c4.push_back({"\"整座玄铁城的商路经营权。外加——你停止收集龙骨。\"", 41});
        c4.push_back({"\"我不下棋——也不谈价。我是来终止这一切的。\"", 43});
        c4.push_back({"回到城门口", 0});
        page4.choices = c4;
    } else if (playerClass == "盗贼") {
        page4.text = "城中最高处，四面无墙的高台。\n"
                     "一个穿月白汉服的男人坐在石凳上，面前摆着残棋。\n\n"
                     "你第一眼看的不是他的脸，不是棋盘——\n"
                     "是他石凳下面那块略微凸起的石板。\n"
                     "那下面有东西。体积不大——一本书，或者一个盒子。\n\n"
                     "\"你的眼睛在转。\"青玉案抬起头。\n"
                     "\"你在找东西。不是找我——是找我藏的东西。\"\n\n"
                     "他把一枚白子放在棋盘正中央。\n"
                     "\"你这种人我见过。不多。但每一个——\n"
                     "都比那些自诩正义的人更容易打交道。\n"
                     "因为你知道——什么都可以偷，什么都可以交易。\n"
                     "包括真相。\"";
        QVector<StoryChoice> c4;
        c4.push_back({"\"你凳子下面藏的是什么？龙骨的配方？\"", 41});
        c4.push_back({"\"我不是来偷东西的——我是来还钥匙的。\"", 44});
        c4.push_back({"回到城门口", 0});
        page4.choices = c4;
    }
    pages.insert(4, page4);

    StoryPage page41;
    page41.text = "青玉案终于落下了那枚棋子。\n"
                  "白子落在棋盘正中央——不是按规则下的位置。\n\n"
                  "\"我在做一件所有人都不敢做的事——打开一扇门。\n"
                  "千年前，天地崩裂，元力从地脉中涌出——\n"
                  "那是门第一次开了。但只开了一条缝。\"\n\n"
                  "他看着你，眼神里有一种狂热，\n"
                  "被死死压在平静的表面下。\n\n"
                  "\"我要把门彻底打开。你问我谋划什么——\n"
                  "我谋划的，是让所有人都有资格使用元力。\n"
                  "不是少数除咎师的专利。是所有人。\"\n\n"
                  "\"你觉得我是疯子？也许。\n"
                  "但你知道吗——每次有人骂我疯子，就证明我没有做错。\"";
    page41.choices.push_back({"——出手——", 411});
    pages.insert(41, page41);

    StoryPage page411;
    page411.text = "你出手了。\n\n"
                   "不是等他落下棋子——是在他的指尖刚触到棋盘的那一瞬间。\n"
                   "你的元力从体内每一根细脉中同时涌出，\n"
                   "像数百条溪流汇成一道洪水，\n"
                   "沿着脊柱冲上手臂，在拳锋凝成一个灼热的点。\n\n"
                   "青玉案没有躲。他甚至没有从棋盘上移开目光。\n\n"
                   "他只是抬起了一只手。\n\n"
                   "从他掌心迸发出的元力形成了一道无形的屏障——\n"
                   "你的拳头撞在上面，像撞上了一堵看不见的墙。\n"
                   "拳锋的元力炸开，在高台上掀起一圈气浪，\n"
                   "将满地的黑白棋子吹得四散飞溅。\n\n"
                   "但你看到了——他的手指在微微颤抖。\n"
                   "那道屏障不是完美的。\n"
                   "在他体内，四十七块龙骨——\n"
                   "每一块都是一个死去的除咎师。\n"
                   "每一个死去的除咎师都在反抗他的控制。\n"
                   "他的力量虽大，但不稳定——\n"
                   "就像用四十七根绳子捆在一起的猛兽，\n"
                   "随时可能挣脱。";
    page411.choices.push_back({"——继续——", 412});
    pages.insert(411, page411);

    StoryPage page412;
    page412.text = "你没有给他喘息的机会。\n\n"
                   "第一击被挡住的瞬间，你已经撤步——不是后退，是旋转。\n"
                   "借着屏障反弹的力道，你的身体转了半圈，\n"
                   "另一只手从腰间挥出——不是拳，是掌。\n"
                   "掌心里凝聚的元力比拳锋更集中、更锐利。\n\n"
                   "掌心撞在屏障上的同一个位置。\n"
                   "同一个位置——你两次攻击击中的是同一个点。\n"
                   "这是从疤面那一战中学到的：\n"
                   "任何防御都有弱点，找到那个点，反复击打。\n\n"
                   "屏障裂了。\n"
                   "不是碎裂——是裂了一道缝。\n"
                   "一道只有匕首刀刃那么宽的缝。\n"
                   "但够了。\n\n"
                   "你的第三击——不是拳，不是掌——\n"
                   "是你整个身体撞了进去。\n"
                   "元力在你身前凝成一个楔形，\n"
                   "顺着那道缝隙插入屏障，\n"
                   "然后——炸开。\n\n"
                   "屏障碎了。\n"
                   "青玉案终于从棋盘上抬起了眼。\n"
                   "他的眼神不再是那种让人不安的平静。\n"
                   "第一次——你在他眼里看到了惊讶。";
    page412.choices.push_back({"——终结——", 99});
    pages.insert(412, page412);

    StoryPage page42;
    page42.text = "青玉案的手顿住了。那枚在棋盘上悬了很久的白子——\n"
                  "\"嗒\"地落了下来，落在了错误的位置。\n\n"
                  "这是他第一次下错。\n\n"
                  "\"……你见过他了？那个在黑风洞里坐了八十年的老东西？\"\n\n"
                  "他缓缓站起。月白汉服在夜风中猛然鼓胀——\n"
                  "元力波动以他为中心，像潮水一样向四面八方涌去。\n"
                  "整座高台的蜡烛在同一刹那被压灭了。\n\n"
                  "他的瞳孔在黑暗中发光——幽蓝色的光。\n"
                  "和黑风洞里那个老者眼眶里的光一模一样。\n\n"
                  "\"八年前我杀了他——我的师父。\n"
                  "他教了我一切，唯独不肯告诉我那道门怎么打开。\n"
                  "我花了八年——收集了四十七块龙骨——\n"
                  "只差最后一块。\"\n\n"
                  "他看着你。\n"
                  "\"你的。\"";
    page42.choices.push_back({"——迎战——", 421});
    pages.insert(42, page42);

    StoryPage page421;
    page421.text = "青玉案抬手的瞬间，高台上的空气凝固了。\n\n"
                   "四十七块龙骨在他体内同时亮起——\n"
                   "透过月白汉服，你能看到幽蓝色的光点遍布他的全身，\n"
                   "像一幅用人命织成的星图。\n\n"
                   "他伸出一根手指。\n"
                   "指尖凝聚的元力浓缩成了一个只有米粒大小的点——\n"
                   "但那一点的亮度，压过了满城灯火。\n\n"
                   "你没有躲。因为你也是一个法师。\n\n"
                   "你体内的元力触角在同时间全部展开——\n"
                   "成百上千根看不见的丝线从你的经脉中伸出，\n"
                   "探向空中那一点、探向青玉案体内的四十七个光点、\n"
                   "探向他元力流转的每一条路径。\n\n"
                   "谛观状态之下，你看到了别人看不到的东西：\n"
                   "青玉案体内的四十七块龙骨——\n"
                   "每一块都在以不同的频率震动。\n"
                   "它们不是和谐的。它们在他的体内互相排斥、互相撕裂。\n"
                   "他用强大的意志力强行压制着它们——\n"
                   "但这种压制有一个极限。\n\n"
                   "你找到了那个极限。\n"
                   "在那四十七个震动频率的间隙里——\n"
                   "有一个频率是空的。\n"
                   "那是被他杀死的第四十八个人的位置。\n"
                   "他师父的位置。\n"
                   "那个位置他永远填不上。\n\n"
                   "你的元力触角伸进了那个空隙。";
    page421.choices.push_back({"——共鸣——", 422});
    pages.insert(421, page421);

    StoryPage page422;
    page422.text = "不是攻击。是共鸣。\n\n"
                   "你将你的元力调到了那个空缺的频率——\n"
                   "那个被青玉案杀死却永远无法被吸收的、\n"
                   "他师父的元力频率。\n\n"
                   "四十七块龙骨同时震颤。\n"
                   "它们感应到了那个空缺——\n"
                   "感应到了本该在它们中间却不存在了的那一块。\n\n"
                   "青玉案的手指开始发抖。\n"
                   "那个凝聚了恐怖元力的光点在他的指尖开始不稳定——\n"
                   "不是你的力量在对抗他。\n"
                   "是他自己体内的龙骨在互相撕裂。\n\n"
                   "\"不……不——\"\n\n"
                   "他第一次失去了冷静。\n"
                   "他用尽全力压制体内的暴动，\n"
                   "但四十七块龙骨——四十七个死去的除咎师——\n"
                   "在感应到他们师父的元力频率之后，\n"
                   "同时开始反噬。\n\n"
                   "青玉案的身体开始崩裂。\n"
                   "不是你的攻击造成的——\n"
                   "是他自己的身体承受不住四十七块龙骨的同时暴走。\n"
                   "幽蓝色的光从他皮肤下面透出来，\n"
                   "从他的眼睛里、嘴里、指尖——\n"
                   "从他的每一处毛孔中迸射而出。\n\n"
                   "\"我……只是想……开一扇门……\"\n\n"
                   "他的声音在发抖。\n"
                   "不是恐惧——是不甘。\n\n"
                   "你收了手。\n"
                   "不是不想杀他。\n"
                   "是不需要了。\n"
                   "四十七块龙骨已经夺回了属于它们自己的意志。\n"
                   "它们正在——一块接一块地——熄灭。";
    page422.choices.push_back({"——终结——", 99});
    pages.insert(422, page422);

    StoryPage page43;
    page43.text = "青玉案看着你。你看着他。\n\n"
                  "然后你伸出手——\n"
                  "把棋盘掀了。\n\n"
                  "黑白棋子滚了一地。有一枚黑子从露台边缘掉了下去，\n"
                  "落在万丈深渊中，没有回音。\n\n"
                  "青玉案沉默了三息。\n"
                  "然后他笑了。\n\n"
                  "\"好。好。你是第一个敢掀棋盘的人。\n"
                  "但棋盘可以掀——棋局呢？\n"
                  "棋局在你心里。你没发现吗——\n"
                  "从你踏进桃源村的那一刻起，\n"
                  "你走的每一步，都是我已经算好的。\"";
    page43.choices.push_back({"\"那你算到这一步了吗？\"", 411});
    pages.insert(43, page43);

    StoryPage page44;
    page44.text = "青玉案的表情终于出现了变化。\n"
                  "不是愤怒，不是恐惧——\n"
                  "而是某种很难形容的，像是琴弦被拨动了一下的微微错愕。\n\n"
                  "\"……卿伊瑟？\"\n\n"
                  "他重复了一遍这个名字，语气很奇怪——\n"
                  "像是在念一个咒语的最后一环。\n\n"
                  "\"你以为你拿她来谈，我会退让？\n"
                  "你错了。这盘棋里，她是唯一一个我没算过的棋子。\n"
                  "不是因为我不能——是因为我不愿。\"\n\n"
                  "他转过身，背对着你，声音忽然变得很低。\n"
                  "\"她长得太像一个人。一个我杀过的人。\n"
                  "你走吧。今晚的棋下到这里。\n"
                  "下次见面——我不会让你走。\"";
    page44.choices.push_back({"（今晚不走——就在这里终结）", 99});
    page44.choices.push_back({"离开高台", 0});
    pages.insert(44, page44);

    // ---- 最终对决·共通路 ----
    StoryPage page99;
    page99.text = "高台之上，风停了。\n\n"
                  "青玉案站在你面前，月白汉服已经被元力撕裂了大半。\n"
                  "他的身体里嵌着四十七块龙骨——\n"
                  "每一块都在发出幽蓝色的光，像四十七只眼睛。\n\n"
                  "但光在衰退。一块接一块地暗下去。\n\n"
                  "\"你赢了。\"\n\n"
                  "他说这三个字的时候，语气很平静。\n"
                  "不是认输的平静——是终于结束了某种煎熬的平静。\n\n"
                  "他闭上了眼。最后一块龙骨熄灭了。\n\n"
                  "晨光从东方升起。残局散落一地——\n"
                  "但在棋盘正中央，多了一枚透明的棋子。\n"
                  "像一颗凝固的橘子瓣。";
    // 按职业跳转不同结局
    if (playerClass == "战士")
        page99.choices.push_back({"—— 结局 ——", 991});
    else if (playerClass == "刺客")
        page99.choices.push_back({"—— 结局 ——", 992});
    else if (playerClass == "法师")
        page99.choices.push_back({"—— 结局 ——", 993});
    else if (playerClass == "富商")
        page99.choices.push_back({"—— 结局 ——", 994});
    else if (playerClass == "盗贼")
        page99.choices.push_back({"—— 结局 ——", 995});
    pages.insert(99, page99);

    // ======== 战士结局：镇关 ========
    StoryPage page991;
    page991.text = "大战之后，玄铁城的烽火终于熄了。\n\n"
                   "铁峰把守卫队长的铁徽章递到你面前。\n"
                   "这枚徽章被磨得发亮——上面有刀痕，有汗渍，有一道淡淡的旧血印。\n\n"
                   "\"这玩意儿戴了十几年了。早该换人。你拿着。\"\n\n"
                   "你看着手里的铁徽章。\n"
                   "\"你不怕我搞砸了？\"\n"
                   "\"搞砸了也是你的。反正老子不管了——我要去喝酒。\"\n\n"
                   "铁峰咧嘴一笑，转身朝酒馆走去。走了三步，又回头：\n"
                   "\"帮我看好城门。别让不该进的人进来。\"\n\n"
                   "你坐在城门口的岗亭里，把徽章别在胸口。\n"
                   "青石镇的青石板大道上，商旅来来往往，\n"
                   "叫卖声、议价声、孩子的笑声，混着风吹过旗帜的猎猎声。\n"
                   "从这里看出去，你能看到桃源村的炊烟，在很远的地方升起。\n\n"
                   "守一扇门。听起来很简单。\n"
                   "但你知道——这扇门后面，是无数个像覃爷爷、田姨、庄叔那样的人。\n"
                   "一扇门，就够了。\n\n"
                   "——【 战士 · 镇关 】——";
    page991.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(991, page991);

    // ======== 刺客结局：影皇 ========
    StoryPage page992;
    page992.text = "你杀了三个人。\n\n"
                   "第一个是疤面。在黑风洞——一刀封喉。\n"
                   "第二个是萧正阳。在玄铁殿背后的暗室里——没有人知道你进去过。\n"
                   "第三个是谁？你不会告诉任何人。\n\n"
                   "因为第三个死掉的，是你从前认识的那个人。\n"
                   "从今以后活着的——只是一个影子。\n\n"
                   "卿伊瑟知道你是谁。所以她从来不问。\n"
                   "鱼池边，她最后一次跟你说话：\n"
                   "\"你欠我的那份人情——别忘了。\"\n"
                   "你当然没忘。但你不知道怎么还。\n"
                   "一个影子，要怎么还人情？\n\n"
                   "从此南境没有你的名字，但每一件大事背后，\n"
                   "都能找到一个影子。\n\n"
                   "——【 刺客 · 影皇 】——";
    page992.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(992, page992);

    // ======== 法师结局：棋手 ========
    StoryPage page993;
    page993.text = "青玉案死了。但棋盘还在。\n\n"
                   "你从他倒下的地方捡起那枚落错位置的白子，放回棋篓。\n"
                   "然后你重新审视这局残棋。\n"
                   "三天。你在高台上坐了三天。\n\n"
                   "第一天，你复盘了他下过的每一步——\n"
                   "八年前他杀了师父，窃取了师父的龙骨，踏上了一条不该走的路。\n"
                   "第二天，你研究了他至死都没想出来的那步棋——\n"
                   "不是技术难题，是心境。他太聪明了，聪明到不肯相信最简单的答案。\n"
                   "第三天，你落子了。\n\n"
                   "天地变色。元力从四面八方涌来——\n"
                   "不是被你召唤，是被这一步棋所代表的\"规则\"唤醒。\n"
                   "你终于明白了：下棋的人从来不是青玉案，也不是你——\n"
                   "是南柯大陆本身。\n\n"
                   "你笑了笑，轻轻将棋盘翻了。\n"
                   "\"不下棋了。我种树。\"\n\n"
                   "从此你隐入苍岭山脉，用元力在焦土上种了一千棵树。\n"
                   "有人在大雨后看到彩虹尽头有一个穿长袍的身影——\n"
                   "须发皆白，但站在树丛中，像一个少年。\n\n"
                   "——【 法师 · 棋手 】——";
    page993.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(993, page993);

    // ======== 富商结局：买天下 ========
    StoryPage page994;
    page994.text = "你花了三个月——\n\n"
                   "——买下了整座青石镇的粮仓和盐仓。\n"
                   "——花了一年，买通了玄铁城七成的官员。\n"
                   "——在他们意识到自己被收购之前，已经买到了萧正阳最后三个幕僚。\n\n"
                   "萧正阳投降那天，你给他的不是战书——\n"
                   "是一张长长的清单，上面列着每个将领的卖身契。\n"
                   "其中有一个名字旁边的数字是零——那是铁峰。铁峰没有被买通。\n\n"
                   "你在大殿上把清单铺开。\n"
                   "\"这世上没有不能谈的买卖。\"\n\n"
                   "你坐在玄铁城的铁椅上，翘着二郎腿，手里转着两颗核桃——\n"
                   "一颗深色，一颗浅色。\n"
                   "\"深色这颗是南境。浅色这颗是整个南柯。下一个要买什么？\"\n\n"
                   "血狼站在你身旁，斧头磨得雪亮。\n"
                   "\"老板——买不动的时候，咱们就抢。\"\n\n"
                   "\"抢？\"你笑了。\"抢是最后的手段。在那之前——先用钱砸。\"\n\n"
                   "——【 富商 · 买天下 】——";
    page994.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(994, page994);

    // ======== 盗贼结局：无间 ========
    StoryPage page995;
    page995.text = "没人知道你是哪边的。\n\n"
                   "侠道的人以为你是他们的人——你在密林帮猎户找过弓。\n"
                   "邪道的人也以为你是他们的人——你在黑风洞跟探子一起偷过酒。\n"
                   "但你心里清楚得很——你哪边都不是。\n"
                   "你只是喜欢这道夹缝。\n\n"
                   "夹缝里，谁也抓不住你。\n"
                   "你可以上午跟赤眉谈武器供应，下午跟卿伊瑟喝茶喂鱼，\n"
                   "晚上再溜进地牢用老鬼给的钥匙放几个你想放的人。\n"
                   "没有人知道是你放的——因为你从来不留下钥匙。\n\n"
                   "有一天你收到一封信。信封上是橘子皮的香味——那个旅人。\n"
                   "信上写：「逍遥够了没有？回来喝酒。——老鬼」\n\n"
                   "你回了一封信。信上只有五个字：\n"
                   "「再偷一座城就回。」\n\n"
                   "老鬼收到信后笑了半天，金牙在烛火下闪闪发光。\n"
                   "\"这个疯子。\"他说这话的时候，语气里全是赞赏。\n\n"
                   "——【 盗贼 · 无间 】——";
    page995.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(995, page995);

    // ======== 战士枭雄结局：独王 (9912) ========
    StoryPage page9912;
    page9912.text = "杀穿玄铁殿的那天，你浑身浴血。\n\n"
                    "从殿门口到铁椅——一百步。你走了九十七步，击倒了十七个禁卫军。\n"
                    "剩下三步，萧正阳自己站起来了。\n\n"
                    "他没出手。他只是看着你。\n"
                    "\"你是第一个让我不想动手的人。不是因为怕你——\n"
                    "因为你是对的。拳头大的，说了算。这套规矩——是我自己立的。\"\n\n"
                    "他把铁椅让出来。\n"
                    "\"坐。坐上去之后你才会知道——这不是王座，是牢笼。\"\n\n"
                    "你坐了。你身上的血还没干。\n\n"
                    "——【 战士 · 独王 】——";
    page9912.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(9912, page9912);

    // ======== 刺客普通结局：无影 (9922) ========
    StoryPage page9922;
    page9922.text = "你从卿伊瑟的庭院出来时，天还没亮。\n\n"
                    "她站在鱼池边。手里的鱼食已经喂完了，但还是保持着伸手的姿势。\n"
                    "\"你会回来吗？\"\n\n"
                    "你没有回答。因为你也不知道。\n\n"
                    "刺客的宿命——是在完成使命之后消失。\n"
                    "就像刀划过水面，水会有痕迹吗？有。但只有一瞬间。\n\n"
                    "你完成了一生中最重要的使命——不是杀人，是救人。足够了。\n\n"
                    "晨雾从城墙外涌进来，穿过街巷，穿过宫殿，穿过庭院。\n"
                    "你在雾中越走越远，直到没有人能分清——那到底是人影，还是雾。\n\n"
                    "——【 刺客 · 无影 】——";
    page9922.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(9922, page9922);

    // ======== 法师不跪结局：问道 (9932) ========
    StoryPage page9932;
    page9932.text = "青玉案死后，你在那座高台上坐了整整一夜。\n\n"
                    "残局还在那里。只是风吹歪了一颗棋子——\n"
                    "把它从错位吹回了对的位置。\n"
                    "你忽然明白了：那一步，就是青玉案一辈子都在找的东西。\n\n"
                    "你抬手，将一枚白子落在棋盘正中央——\n"
                    "不是破坏规则，而是完成规则。\n"
                    "棋盘微微一震。元力从棋子间逸散开来，在空中凝成一行极淡的字：\n"
                    "「天下兴亡，匹夫有责。——答《残局问》」\n\n"
                    "字迹在空中停留了三息，被晨风吹散了。\n"
                    "你起身，拂袖而去。\n\n"
                    "从此世间少了一个宫廷法师，多了一个云游四方的问道者。\n"
                    "有人偶尔在南境的某个偏远小村看到过他——\n"
                    "他在帮一个老药铺的妇人熬药。\n\n"
                    "——【 法师 · 问道 】——";
    page9932.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(9932, page9932);

    // ======== 富商普通结局：富甲 (9942) ========
    StoryPage page9942;
    page9942.text = "三个月后。青石镇的商道上建起了一座新的驿站。\n\n"
                    "驿站的牌匾上只写了一个字——「信」。\n"
                    "意思是：在我这里，诚信比钱贵。\n\n"
                    "因为你打通了桃源村到玄铁城的商路，\n"
                    "从此粮车畅通，山贼绝迹。\n"
                    "那条曾经让人闻风丧胆的密林小路，现在路边每隔一里就有一个茶摊。\n"
                    "茶摊的老板娘说，是一个不愿透露姓名的商人出钱修的。\n\n"
                    "村民们叫你「萧老板」——虽然你并不姓萧。\n"
                    "你说无所谓，名字只是个招牌。\n\n"
                    "那天你坐在驿站二楼的窗边，手里端着茶杯。\n"
                    "窗外，庄叔的铁匠铺新进了一批铁矿，正冒着热气。\n"
                    "田姨的药铺开了一家分号，就在驿站隔壁。\n"
                    "覃爷爷坐在老槐树下，蒲扇摇啊摇。\n\n"
                    "\"这世上最值钱的不是钱。\"你对着窗外说。\n"
                    "\"是路。没有路，所有的钱都是废铁。\"\n\n"
                    "——【 富商 · 富甲 】——";
    page9942.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(9942, page9942);

    // ======== 盗贼普通结局：逍遥 (9952) ========
    StoryPage page9952;
    page9952.text = "玄铁城事了之后，你收到了一封信。\n\n"
                    "信封上没有署名，但封口处别着一根干了的橘子皮丝。\n"
                    "你认得这个味道。那个在桃源村老槐树下剥橘子的旅人——\n"
                    "原来他还在看着你。\n\n"
                    "信上只有一行字：\n"
                    "「手法比以前好了。有空来青石镇喝茶。——老鬼」\n\n"
                    "你笑了。把信纸折成一只纸飞机，从玄铁城最高的城墙上扔了出去。\n"
                    "纸飞机乘着风，飞过密林的树梢，飞过青石镇的钟楼，\n"
                    "飞过黑风洞顶的乌鸦巢，最后——落在桃源村田姨的药铺屋顶上。\n\n"
                    "而你——在纸飞机落地之前，已经不在城墙上了。\n\n"
                    "地图上，找不到你的名字。\n"
                    "但如果你仔细听——每个小镇的酒馆里，都有人在讲一个故事。\n"
                    "故事的主角没有名字，只有外号。\n\n"
                    "——【 盗贼 · 逍遥 】——";
    page9952.choices.push_back({"—— 返回主菜单 ——", -1});
    pages.insert(9952, page9952);

    screenControl->setStories(pages);
}

void city::resetStory() { screenControl->resetStory(); }
void city::onReturnToParent() { emit Return(); screenTransition->hideScreen(this); }
void city::onSceneActionRequested(const QString& action) { screenTransition->hideScreen(this); emit navigateTo(action); }
void city::on_button_back_clicked(bool) { emit Return(); screenTransition->hideScreen(this); }
void city::on_button_next_clicked(bool) { travel *d = new travel(this); connect(d, &travel::destinationSelected, this, &city::navigateTo); connect(d, &travel::destinationSelected, d, &QDialog::deleteLater); d->exec(); }
void city::on_button3_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) { track("lord_choice",11); ui->button_next->setVisible(true); } if(c==2) { track("princess_choice",21); ui->button_next->setVisible(true); } if(c==3) { track("rebel_choice",31); ui->button_next->setVisible(true); } if(c==4) { track("qingyuan_choice",41); ui->button_next->setVisible(true); } if(c==412||c==422||c==44) updateEnding(); screenControl->onChoiceClicked(0); if(screenControl->getCurrentPage()==0) refreshHub(); }
void city::on_button4_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("lord_choice",12); if(c==2) track("princess_choice",22); if(c==3) track("rebel_choice",32); if(c==4) track("qingyuan_choice",42); screenControl->onChoiceClicked(1); if(screenControl->getCurrentPage()==0) refreshHub(); }
void city::on_button5_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("lord_choice",13); if(c==2) track("princess_choice",23); if(c==3) track("rebel_choice",33); if(c==4) track("qingyuan_choice",43); screenControl->onChoiceClicked(2); if(screenControl->getCurrentPage()==0) refreshHub(); }
void city::on_button6_clicked(bool) { int c=screenControl->getCurrentPage(); if(c==1) track("lord_choice",14); if(c==2) track("princess_choice",24); if(c==3) track("rebel_choice",34); if(c==4) track("qingyuan_choice",44); screenControl->onChoiceClicked(3); if(screenControl->getCurrentPage()==0) refreshHub(); }
void city::on_button7_clicked(bool) { screenControl->onChoiceClicked(4); }
void city::updateEnding() { if(!pages.contains(99)) return; StoryPage& p=pages[99]; p.choices.clear(); if(playerClass=="战士"){ if(st("mayor_choice").toInt()==11||st("bamian_choice").toInt()==11) p.choices.push_back({"—— 侠道 · 镇关 ——", 991}); else p.choices.push_back({"—— 独王 ——", 9912}); } else if(playerClass=="刺客"){ if(st("princess_choice").toInt()==24) p.choices.push_back({"—— 影皇 ——", 992}); else p.choices.push_back({"—— 无影 ——", 9922}); } else if(playerClass=="法师"){ if(st("mage_kneel").toBool()) p.choices.push_back({"—— 棋手 ——", 993}); else p.choices.push_back({"—— 问道 ——", 9932}); } else if(playerClass=="富商"){ if(st("xuelang_choice").toInt()==43) p.choices.push_back({"—— 买天下 ——", 994}); else p.choices.push_back({"—— 富甲 ——", 9942}); } else if(playerClass=="盗贼"){ if(st("heishi_choice").toInt()==23) p.choices.push_back({"—— 无间 ——", 995}); else p.choices.push_back({"—— 逍遥 ——", 9952}); } }
void city::track(const QString& k, const QVariant& v) { m_state[k]=v; }
QVariant city::st(const QString& k) const { return m_state.value(k); }
void city::refreshHub() { if(!pages.contains(0)) return; StoryPage& p=pages[0]; p.choices.clear(); p.choices.push_back({"前往玄铁殿（领主·萧正阳）", 1}); p.choices.push_back({"前往公主庭院（卿伊瑟）", 2}); p.choices.push_back({"寻找叛军（赤眉）", 3}); p.choices.push_back({"登上高台（青玉案）", 4}); if(st("lord_choice").toInt()==11||st("lord_choice").toInt()==12) p.choices.push_back({"（获得领主信任）登上高台——终结这一切", 99}); if(st("princess_choice").toInt()==24) p.choices.push_back({"（持有禁军调令）登上高台", 99}); if(st("rebel_choice").toInt()==33||st("rebel_choice").toInt()==34) p.choices.push_back({"（叛军支援）攻上高台", 99}); p.choices.push_back({"返回青石镇", -2, "town"}); }
