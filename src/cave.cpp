#include "cave.h"
#include "ui_cave.h"
#include "travel.h"
#include "musicmanager.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

cave::cave(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::cave)
    , screenTransition(new ScreenTransition(this))
    , screenControl(new ScreenControl(this))
    , playerClass("")
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/cave_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "cave");

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
            this, &cave::onReturnToParent);
    connect(screenControl, &ScreenControl::sceneActionRequested,
            this, &cave::onSceneActionRequested);

    rebuildStory();
    screenControl->resetStory();
}

cave::~cave()
{
    delete ui;
}

void cave::setPlayerClass(const QString& className)
{
    playerClass = className;
    rebuildStory();
    screenControl->resetStory();
}

void cave::rebuildStory()
{
    pages.clear();

    // ---- 第 0 页：洞窟入口 ----
    StoryPage page0;
    page0.text = "山壁裂开一道巨大的口子，像被一柄看不见的巨斧劈开的伤疤。\n\n"
                 "洞口站着两个山贼，手里的火把照得人脸忽明忽暗。\n"
                 "石壁上刻着三个歪歪扭扭的大字——「黑风洞」。\n\n"
                 "洞内阴风阵阵，不知从哪个岔路口吹来的气流\n"
                 "带着一股说不清的味道——像铁锈，像腐肉，\n"
                 "又像某种沉睡了很久很久的东西。\n\n"
                 "深处的黑暗中，隐约传来人声和锁链拖地的回响。\n\n"
                 "洞口守卫看到你，手按上了刀柄。";
    // 按职业定制入口
    if (playerClass == "战士") {
        page0.text += "\n\n你握紧了兵器。对你来说，进洞只有一种方式——从正门。";
        page0.choices.push_back({"\"我来见疤面。\"", 1});
        page0.choices.push_back({"（不废话，直接闯进去）", 2});
    } else if (playerClass == "刺客") {
        page0.text += "\n\n你在阴影中观察着洞口。守卫浑然不觉——你可以在他们发现之前，\n就已站在疤面身后。";
        page0.choices.push_back({"（从阴影中无声潜入——直取疤面后颈）", 14});
        page0.choices.push_back({"\"先礼后兵——我来见疤面。\"", 1});
    } else if (playerClass == "法师") {
        page0.text += "\n\n你感应到洞内深处传来一股不寻常的元力波动——\n古老而微弱的符文共鸣，像是有什么在呼唤你。";
        page0.choices.push_back({"（法师专属）循着元力波动——先去岔路尽头看看", 4});
        page0.choices.push_back({"\"先见疤面，岔路的事稍后再说。\"", 1});
    } else if (playerClass == "盗贼") {
        page0.text += "\n\n你一眼就看到了洞侧高处那个通风口——\n窄得刚好能容一人侧身通过。守卫根本没注意到它。";
        page0.choices.push_back({"（盗贼专属）从侧面的通风口溜进去", 3});
        page0.choices.push_back({"\"先正面会会这个疤面。\"", 1});
    } else {
        // 富商
        page0.choices.push_back({"\"我来见疤面。\"", 1});
        page0.choices.push_back({"（不废话，直接闯进去）", 2});
    }
    page0.choices.push_back({"返回密林", -2, "forest"});
    pages.insert(0, page0);

    // ---- 第 1 页：面见疤面（按职业定制） ----
    StoryPage page1;
    if (playerClass == "战士") {
        page1.text = "洞穴最深处，一个独眼男人坐在石椅上。\n"
                     "他身旁插着一柄与人同高的鬼头大刀。\n\n"
                     "你的目光没有看铁笼，没有看那把刀——你看着他。\n"
                     "他也看着你。两个战士在动手之前，都会先看对方的眼睛。\n\n"
                     "\"……有意思。\"他缓缓站了起来。\n"
                     "\"大部分人进来之后，第一眼看的是笼子里的姑娘。\n"
                     "你第一眼看的是我的刀。\"\n\n"
                     "他的手按上了刀柄。不是威胁——是尊重。\n"
                     "\"我叫疤面。打了二十年仗，输了七场，赢了数不清。\n"
                     "你这种人——我一眼就认得出来。\n"
                     "说吧。是来杀我的，还是来打一架的？\"";
        page1.choices.push_back({"\"都不是。我是来带她走的。\"", 11});
        page1.choices.push_back({"\"（战士专属）人我要带走。让路，或者死。\"", 13});
        page1.choices.push_back({"回到洞口", 0});
    } else if (playerClass == "刺客") {
        page1.text = "洞穴最深处，一个独眼男人坐在石椅上。\n"
                     "鬼头大刀靠在他右手边——拔刀需要一瞬间。\n"
                     "四步。从你现在的位置到他后颈，最多四步。\n\n"
                     "但他感觉到了。他的独眼在黑暗中闪烁了一下，\n"
                     "手不自觉地往刀柄的方向移了半寸。\n\n"
                     "\"……你站的位置不对。太近了。太安静了。\"\n"
                     "他抬起独眼，第一次正眼看你。\n"
                     "\"我见过你这种人。一共三个。前面两个——\n"
                     "我活下来了，他们没有。\"\n\n"
                     "他指了指铁笼。\n"
                     "\"你是为她来的？还是为刀上的赏金来的？\"";
        page1.choices.push_back({"\"为她。放人。\"", 11});
        page1.choices.push_back({"（沉默地评估距离——四步，够吗？）", 14});
        page1.choices.push_back({"回到洞口", 0});
    } else if (playerClass == "法师") {
        page1.text = "洞穴最深处，一个独眼男人坐在石椅上。\n"
                     "鬼头大刀插在他身旁——刀身上附着的元力波动\n"
                     "在你踏入十步之内的时候就引起了你的注意。\n\n"
                     "那不是一把普通的刀。\n"
                     "它被元力浸过——不是天然浸染，是有人刻意灌注的。\n\n"
                     "\"你在看我的刀。\"疤面开口了。\n"
                     "\"三年前也有一个人这样看我的刀。穿白袍的。\n"
                     "他说我的刀被元力浸过，但我不会用。他可以教我。\n"
                     "代价是——每个月帮他收一批龙骨。\"\n\n"
                     "他盯着你。\n"
                     "\"你跟他是一路的？\"";
        page1.choices.push_back({"\"不是。我是来阻止他的。\"", 11});
        page1.choices.push_back({"\"（法师专属）你的刀——谁给你的元力？\"", 16});
        page1.choices.push_back({"回到洞口", 0});
    } else if (playerClass == "富商") {
        page1.text = "洞穴最深处，一个独眼男人坐在石椅上。\n"
                     "你第一眼看的不是他——是他洞里的货。\n"
                     "角落里堆着几箱从青石镇抢来的绸缎和茶叶。\n"
                     "品相不错，但保存得不好——潮了。浪费。\n\n"
                     "\"你看的不是姑娘，不是刀——是货。\"\n"
                     "疤面笑了。这是他今天第一次笑。\n\n"
                     "\"你是生意人。好。生意人比侠客好打交道——\n"
                     "至少你知道什么东西值钱。\"\n\n"
                     "他指了指铁笼。\n"
                     "\"这个姑娘——玄铁城卿家的人。赎金不会少。\n"
                     "但我不认识玄铁城的人。你认识？\"";
        page1.choices.push_back({"\"我认识。开个价——我们谈笔更大的。\"", 15});
        page1.choices.push_back({"\"放了她。我可以帮你把龙骨的生意做大。\"", 11});
        page1.choices.push_back({"回到洞口", 0});
    } else if (playerClass == "盗贼") {
        page1.text = "洞穴最深处，一个独眼男人坐在石椅上。\n"
                     "你进来的时候顺便数了一下——从洞口到这里，\n"
                     "一共经过了三个岔路口。其中有一个是死路，\n"
                     "但死路尽头有一道没有锁的铁门。\n\n"
                     "那是疤面的私人酒窖。你没进去——但你记住了位置。\n\n"
                     "\"你的眼睛在转。\"疤面开口了。\n"
                     "\"你不是在看我的刀，也不是在看笼子——\n"
                     "你在看我的洞。你在找东西。\"\n\n"
                     "他笑了一声，独眼里有一丝赏识。\n"
                     "\"有意思。我这里除了人命，没什么值钱的。\n"
                     "除非——你是来偷我酒窖的？\"";
        page1.choices.push_back({"\"我没偷——但你的酒窖门锁不行。建议换一把。\"", 11});
        page1.choices.push_back({"\"我是来帮你解决问题的——顺便谈个条件。\"", 17});
        page1.choices.push_back({"回到洞口", 0});
    }
    pages.insert(1, page1);

    // ---- 疤面分支 11：劝放（理性） ----
    StoryPage page11;
    page11.text = "疤面的独眼眯了起来。他看了你很久，然后忽然笑了——\n"
                  "笑得很慢，像是一扇生锈的门被推开。\n\n"
                  "\"有意思。你是第一个跟我聊道理的。\"\n\n"
                  "他坐回石椅上，把鬼头刀随手立在一边。\n"
                  "\"我疤面杀人放火二十年——但没杀过一个不该杀的人。\n"
                  "这姑娘我本来也没打算留。不过——\n"
                  "既然是你开的口，这个人情算在你头上。\"\n\n"
                  "他从怀里摸出一把钥匙，扔了过来。\n"
                  "\"放人。送他们出去。\"\n\n"
                  "瑶儿被从铁笼中放出，她看了你一眼，没有说话。\n"
                  "但那一眼里有太多东西——感激、疑问、和一份沉甸甸的委托。";
    page11.choices.push_back({"护送瑶儿去青石镇", -2, "town"});
    page11.choices.push_back({"先去洞内深处看看", 5});
    pages.insert(11, page11);

    // ---- 疤面分支 12：不放（强硬） ----
    StoryPage page12;
    page12.text = "疤面的独眼里闪过一丝意外。然后他笑了——这次是真的在笑。\n\n"
                  "\"好。够狠。我喜欢。\"\n\n"
                  "他从石椅上站起来，第一次正眼看你。\n"
                  "\"我手底下二十几个兄弟，没有一个敢这么跟我说话。\n"
                  "你留下——副寨主的位置给你。\"\n\n"
                  "他指了指铁笼。\n"
                  "\"至于她——卖到玄铁城，能换一笔不小的赎金。\n"
                  "你拿三成。怎么样？\"";
    page12.choices.push_back({"\"成交。\"（入伙黑风洞）", -2, "town"});
    page12.choices.push_back({"\"我改主意了——放人。\"", 11});
    pages.insert(12, page12);

    // ---- 疤面分支 13：战士专属 ----
    StoryPage page13;
    page13.text = "疤面慢慢地、慢慢地站了起来。\n\n"
                  "他的手握住了鬼头刀的刀柄。你能听到他指节发力的声音——\n"
                  "那是二十年的老茧压在二十年磨出的刀柄纹路上的摩擦声。\n\n"
                  "\"二十年了。上一次有人敢在我面前说这种话——\n"
                  "是一个青石镇的守卫队长。叫铁峰。\n"
                  "我跟他打了三十回合，没分出胜负。\"\n\n"
                  "他拔出鬼头刀。刀尖在地上拖出一道火星，\n"
                  "在黑暗中划出一条短暂的光路。\n"
                  "\"你——够不够格让我出刀？\"\n\n"
                  "你没有回答。你只是往前踏了一步。\n"
                  "这一步里没有什么花哨——\n"
                  "只是将全身的重量从后脚移到前脚，\n"
                  "膝盖微沉，重心下沉三寸。\n"
                  "脚下的石地裂了一道缝。\n"
                  "裂缝从你脚底蔓延到疤面脚边，\n"
                  "像一条无声的战书。\n\n"
                  "疤面看着那道裂缝，独眼里闪过一丝光——\n"
                  "不是恐惧。是一种被压了二十年、\n"
                  "终于被唤醒了的东西。\n\n"
                  "\"好。\"\n\n"
                  "他说这个字的时候，鬼头刀已经举过了头顶。";
    page13.choices.push_back({"——迎战——", 131});
    pages.insert(13, page13);

    StoryPage page131;
    page131.text = "鬼头刀劈下的那一瞬，你没有躲。\n\n"
                    "你体内的元力触角在谛观状态下完全展开——\n"
                    "上百根看不见的丝线从你的经脉中伸出，\n"
                    "探向刀锋、探向刀柄上那只手、探向手背上每一根青筋的跳动。\n\n"
                    "刀落的速度在你眼里变慢了。\n"
                    "不是真的变慢了——是你的感知变快了。\n\n"
                    "你看到了刀锋劈开空气时产生的细微波纹，\n"
                    "看到了刀身上那道暗褐色痕迹——那不是锈，是干涸了太多次的血。\n"
                    "看到了疤面的独眼里——没有杀意。\n"
                    "只有一种干净的、纯粹的战意。\n\n"
                    "刀锋落下。\n\n"
                    "你侧身——只侧了三寸。\n"
                    "刀锋擦着你的肩膀劈下，斩碎了你脚下的石地。\n"
                    "碎石溅起来，打在你的脸侧，打出几道细小的血痕。\n\n"
                    "你没有退。\n"
                    "在刀锋入地的那一瞬间——\n"
                    "在疤面的力量用老、新力未生的那一瞬间——\n"
                    "你出手了。\n\n"
                    "你的拳头从腰间发出，没有花哨的弧度，\n"
                    "只是一条直线。\n"
                    "拳头的前端是你的指骨，\n"
                    "指骨后面是你整个身体的重量加上旋转的惯性。\n"
                    "元力从丹田升起，沿着脊柱冲上肩膀，\n"
                    "在肘关节凝成一个爆发点，\n"
                    "最后在拳锋炸开。\n\n"
                    "这一拳的目标不是疤面的脸——\n"
                    "是他握刀的手腕。";
    page131.choices.push_back({"——拳落——", 132});
    pages.insert(131, page131);

    StoryPage page132;
    page132.text = "疤面看到了你的拳。\n\n"
                    "他的独眼在那百分之一息的时间里做出了判断——\n"
                    "松刀，退后。\n"
                    "如果他慢一瞬，他的手腕会被你的拳锋击中，\n"
                    "握刀的手会废掉。\n\n"
                    "他松了。鬼头刀落在地上，\n"
                    "刀身和石地碰撞，炸出一串火星。\n\n"
                    "疤面退了三步。每一步都在石地上踩出一个浅坑。\n"
                    "他的后背撞在石椅上，石椅裂了。\n\n"
                    "你的拳头停在了他鼻尖前一寸。\n"
                    "拳风把他额前的头发吹了起来。\n\n"
                    "洞窟里安静了。\n"
                    "只有火把燃烧的噼啪声，\n"
                    "和两个人粗重的呼吸声。\n\n"
                    "疤面的独眼看着你的拳锋——\n"
                    "然后他笑了。不是之前的冷笑，\n"
                    "是一个真正的战士遇到另一个战士时的笑。\n\n"
                    "\"够了。\"\n\n"
                    "他喘了一口气。\n"
                    "\"人你带走。不是我怕你——\n"
                    "是我不想为了一个姑娘，折了一个好对手。\"\n\n"
                    "他弯腰捡起鬼头刀。\n"
                    "手还在发抖——刚才那一拳的余力还没有从他的手腕上消散。\n"
                    "\"下次见面——我们好好打一场。\n"
                    "不赌命，只赌酒。\"";
    page132.choices.push_back({"护送瑶儿去青石镇", -2, "town"});
    page132.choices.push_back({"\"一言为定。\"", 0});
    pages.insert(132, page132);

    // ---- 疤面分支 14：刺客专属 ----
    StoryPage page14;
    page14.text = "你没有说话。\n\n"
                  "沉默在洞窟中蔓延——\n"
                  "比刀锋更锋利，比火把更灼热。\n\n"
                  "疤面的独眼在黑暗中闪烁了一下。\n"
                  "他的手还握在鬼头刀的刀柄上——\n"
                  "但指节已经开始发白。\n\n"
                  "他感觉到了。\n"
                  "那种被刀锋盯住后颈的寒意——\n"
                  "不是来自前方，而是来自他看不见的方向。\n"
                  "作为战士的直觉告诉他：\n"
                  "你看起来站在他面前三步之外，\n"
                  "但真正的威胁——真正的刀——\n"
                  "已经在某个他不确定的位置等着他了。\n\n"
                  "\"……有意思。\"\n\n"
                  "他的声音比刚才低了半度。\n"
                  "\"你这种人我见过。不多——三个。\n"
                  "第一个是青石镇来的，在我睡着的时候摸进了我的洞。\n"
                  "第二天早上我发现枕头边多了一把匕首——不是来杀我的，是来警告我的。\n"
                  "第二个是玄铁城的人，至今没人知道他是怎么进的城。\n"
                  "第三个——\"\n\n"
                  "他没有说完。\n"
                  "他把鬼头刀放回了原位。\n"
                  "不是放下武器——是放弃了拔刀的想法。\n"
                  "因为他知道，在他的手碰到刀柄的那一刻——\n"
                  "你的刀已经在某个更致命的位置了。\n\n"
                  "\"带她走。别让我后悔这个决定。\"\n\n"
                  "你的手从匕首柄上松开了。\n"
                  "你甚至没有意识到你什么时候握住的。\n"
                  "这就是刺客的本能——\n"
                  "在意识做出决定之前，身体已经做出了判断。";
    page14.choices.push_back({"护送瑶儿去青石镇", -2, "town"});
    page14.choices.push_back({"回到洞口", 0});
    pages.insert(14, page14);

    // ---- 疤面分支 16：法师专属 ----
    StoryPage page16;
    page16.text = "疤面的独眼在你提到\"青玉案\"三个字的时候骤然收缩。\n\n"
                   "\"……你果然跟他有关系。\"\n\n"
                   "他的手按上了鬼头刀的刀柄——但那不是一个攻击的动作，\n"
                   "而是一个恐惧的、本能的自保动作。\n\n"
                   "\"三年前他路过我的洞，往我的刀里灌了元力。\n"
                   "从那以后我每个月都得往玄铁城送龙骨。\n"
                   "我不敢不去——你没见过他发怒的样子。\n"
                   "他抬手就能把一个人的元力从体内抽出来。\n"
                   "像抽水一样。干干净净。\"\n\n"
                   "他低头看着自己的鬼头刀。\n"
                   "\"这把刀上的元力——不是我想要的。\n"
                   "你要是能把它去掉，我把我知道的全告诉你。\"";
    page16.choices.push_back({"（用谛观探查刀上的元力封印）", 161});
    page16.choices.push_back({"\"先放人。刀的事——出去再说。\"", 11});
    pages.insert(16, page16);

    StoryPage page161;
    page161.text = "你闭上眼。元力触角从体内伸出，探向刀身。\n\n"
                    "刀里封着一道元力印记——青玉案的印记。\n"
                    "这不是普通的灌注，是\"龙骨锁\"：\n"
                    "一种将元力刻入死物、用以追踪和控制的术法。\n"
                    "青玉案不仅能追踪疤面的位置——\n"
                    "还能通过这道印记感知到附近是否有人识破了它。\n\n"
                    "你睁开眼。\n"
                    "\"我解不了——至少现在解不了。\n"
                    "但我可以告诉你这是什么：龙骨锁。\n"
                    "青玉案用它追踪你，也用它监视你。\n"
                    "你每送一次龙骨，他就在你这把刀上多加一层封印。\n"
                    "等封印加满——你的元力也会被他抽走。\"\n\n"
                    "疤面的独眼里——第一次出现了恐惧之外的什么。\n"
                    "是愤怒。\n"
                    "\"……他骗了我三年。\"";
    page161.choices.push_back({"\"放了瑶儿——我们一起对付他。\"", 11});
    page161.choices.push_back({"前往青石镇寻找破解之法", -2, "town"});
    pages.insert(161, page161);

    // ---- 疤面分支 17：盗贼专属 ----
    StoryPage page17;
    page17.text = "疤面盯着你看了很久。然后他笑了——\n"
                   "一种被看穿了反而觉得有趣的笑。\n\n"
                   "\"你知道吗——你这种人，用来守酒窖浪费了。\n"
                   "我有个提议：你帮我做一件事，\n"
                   "我放了这姑娘，还送你一坛好酒。\"\n\n"
                   "他凑近了，压低声音。\n"
                   "\"青石镇的黑市商人老鬼——欠我一笔账。\n"
                   "不多，三百金币。但他拖了两年。\n"
                   "你去帮我把账收回来——不管你用什么办法。\n"
                   "收到之后你拿三成。剩下的归我。\"\n\n"
                   "他从怀里摸出一张皱巴巴的欠条。\n"
                   "\"怎么样？这笔买卖——比偷酒窖划算吧？\"";
    page17.choices.push_back({"\"成交。但我先带人走——欠条我收着。\"", 11});
    page17.choices.push_back({"\"三百金币——老鬼欠的？这笔账我帮你收。\"", -2, "town"});
    page17.choices.push_back({"回到洞口", 0});
    pages.insert(17, page17);

    // ---- 疤面分支 15：富商专属 ----
    StoryPage page15;
    page15.text = "疤面的独眼从你脸上移到了你的钱袋上，\n"
                  "又从钱袋回到了你的脸上。\n\n"
                  "\"生意？什么生意比这姑娘的赎金还大？\"\n\n"
                  "\"龙骨的生意。\"\n\n"
                  "疤面的表情变了。那是你第一次在他的独眼里看到——恐惧。\n\n"
                  "\"你怎么知道龙骨？\"\n\n"
                  "\"我不但知道龙骨——我还知道你每个月往玄铁城送一批。\n"
                  "送给一个叫青玉案的法师。\"\n\n"
                  "疤面沉默了很久。洞里的风忽然变得更冷了。\n\n"
                  "\"……你到底是什么人？\"\n\n"
                  "\"一个想跟你做生意的商人。放了她——我帮你把龙骨的生意做大。\"";
    page15.choices.push_back({"继续谈判……", -2, "town"});
    pages.insert(15, page15);

    // ---- 第 2 页：直接闯进去 ----
    StoryPage page2;
    page2.text = "你没有废话。你直接走进了黑暗。\n\n"
                 "洞口两个守卫同时拔刀。刀身在火把下闪过一瞬冷光——\n"
                 "然后刀光就暗了。\n\n"
                 "你的元力在踏入洞口的瞬间已经涌到指尖。\n"
                 "不待刀锋落下，你已切入两人之间那道只有一掌宽的缝隙——\n"
                 "左肘撞在第一个守卫的刀柄上，刀身偏转，\n"
                 "刀背反砸在他自己的额头上。\n"
                 "右手同时扣住第二个守卫的手腕，\n"
                 "顺着他的力道往下一带——\n"
                 "他连人带刀栽进了石壁的凹陷里。\n\n"
                 "两声闷响在洞穴中回荡了五息才消散。\n"
                 "两个守卫倒在洞口，火把从他们手中滚落，\n"
                 "在地上跳了两下，熄了。\n\n"
                 "洞穴深处的黑暗中，一把鬼头大刀被缓缓拔出。\n"
                 "刀尖在石地上拖出一道火星——\n"
                 "像是黑暗本身在对你龇牙。";
    page2.choices.push_back({"与疤面正面对峙", 1});
    pages.insert(2, page2);

    // ---- 第 3 页：盗贼专属——通风口潜入 ----
    StoryPage page3;
    page3.text = "你在洞窟侧面找到了一个通风口——\n"
                 "窄得只能容一个人侧身通过。\n\n"
                 "你像一条蛇一样滑了进去，没有碰到任何一根悬挂的绳索。\n"
                 "三十息之后，你已经站在了铁笼后面。\n\n"
                 "瑶儿察觉到了身后的动静。她没有回头。\n"
                 "\"你是来救我的——还是来抓我的？\"\n\n"
                 "\"都不是。\"你低声说，\"我只是路过。顺便把你带出去。\"\n\n"
                 "锁开了。没有声音。\n"
                 "疤面还在石椅上喝酒，完全没注意到铁笼已经空了。";
    page3.choices.push_back({"带瑶儿离开去青石镇", -2, "town"});
    pages.insert(3, page3);

    // ---- 第 4 页：法师专属——探岔路 ----
    StoryPage page4;
    page4.text = "你没有走向主洞。你循着元力波动，\n"
                 "钻进了最深处的一条死路。\n\n"
                 "那里盘坐着一个白发老者。衣服已经烂得不成样子，\n"
                 "露出的皮肤上刻满了密密麻麻的符文——\n"
                 "那些符文已经和皮肤融为一体，像根系一样扎进肉里。\n\n"
                 "他闭着眼，一动不动——直到你踏入十步之内。\n"
                 "所有符文同时亮了起来。\n\n"
                 "\"你是来学东西的，还是来杀我的？\n"
                 "先回答——省得浪费口舌。\"\n\n"
                 "\"要是来杀我——省省力气。这具身体八十年前就已经半死了。\n"
                 "要是来学东西——跪下。我只说三句话。\"";
    page4.choices.push_back({"（跪下）请前辈赐教", 41});
    page4.choices.push_back({"我不跪任何人", 42});
    pages.insert(4, page4);

    StoryPage page41;
    page41.text = "你跪下了。膝盖触到冰冷石块的瞬间，\n"
                   "你感受到一阵刺痛——不是身体上的，\n"
                   "而是元力在体内每一条细脉中同时翻涌。\n\n"
                   "老者睁眼了。眼眶里没有眼珠，只有两团幽蓝色的光。\n\n"
                   "\"第一句——元力不是力量，是路。\n"
                   "每个人体内都有成千上万条小路。\n"
                   "大多数人一辈子只走了一条，还走错了。\n"
                   "找到你自己的路，别走别人的。\"\n\n"
                   "\"第二句——洞最深处，有一具千年妖兽的骸骨。\n"
                   "它的骨髓是活的。找一个丹炉，把骨髓炼成丹药。\n"
                   "炼成了，法力翻倍；炼不成，折寿十年。\n"
                   "赌不赌——你自己决定。\"\n\n"
                   "\"第三句——\"\n"
                   "他顿住了。幽蓝的光开始剧烈闪烁。\n"
                   "他的身体从手指开始，一寸一寸地化为飞灰。\n\n"
                   "\"玄铁城。青玉案。别信他。一个字也不要信。\"\n\n"
                   "老者化为一堆灰色的粉末。\n"
                   "只有一截指骨留在原地，上面刻着：「吾道不孤」。";
    page41.choices.push_back({"返回主洞见疤面", 1});
    page41.choices.push_back({"前往青石镇", -2, "town"});
    pages.insert(41, page41);

    StoryPage page42;
    page42.text = "老者等了三息。三息之后，他嘴角微动——竟像是在笑。\n\n"
                   "\"不跪。好。有骨气。\n"
                   "但骨气杀不了人，也救不了人。\n"
                   "你知道吗——八十年前，我也跟你一样。\n"
                   "不跪天地，不跪君王，不跪任何人。\"\n\n"
                   "他沉默了一会儿，符文的光慢慢暗了下去。\n\n"
                   "\"后来我为了救一个人跪了。跪了整整三天三夜——\n"
                   "那人还是死了。所以不跪也好。\n"
                   "你走吧——会有别人来的。\"\n\n"
                   "你转身离开。但你知道，\n"
                   "从此以后，任何精神控制的术法对你都不再有用了。\n"
                   "你跪不下去——不是因为傲慢，是因为骨头里有一根刺。";
    page42.choices.push_back({"返回主洞见疤面", 1});
    pages.insert(42, page42);

    // ---- 第 5 页：洞窟深处 ----
    StoryPage page5;
    page5.text = "你深入黑风洞最深处。\n\n"
                 "这里的石壁上有一道巨大的裂缝，\n"
                 "裂缝里向外渗着幽蓝色的光——和老者眼眶里的光一模一样。\n\n"
                 "你看到了那具千年妖兽的骸骨。\n"
                 "它太大了——大到你看不到它的全貌，\n"
                 "只能看到一根根比人还高的肋骨从石壁中刺出来。\n\n"
                 "骨髓还在流动，像活着一样。";
    page5.choices.push_back({"采集妖骸骨髓", 51});
    page5.choices.push_back({"太危险了，离开这里", -2, "town"});
    pages.insert(5, page5);

    StoryPage page51;
    page51.text = "你用匕首割开妖骨的外壳。\n"
                   "幽蓝色的骨髓像熔岩一样渗出来，落在你的手心——\n"
                   "不烫，冰冷。\n\n"
                   "你感受到一股前所未有的元力涌入体内，\n"
                   "像一条大河突然改道，冲进了你身体里的每一条小路。\n\n"
                   "你咬着牙，跪在地上，承受着这股力量的洗礼。\n\n"
                   "当一切平复之后——你站了起来。\n"
                   "你感觉到自己不一样了。\n\n"
                   "（获得「千年妖骸」——可在青石镇寻找炼丹材料炼制丹药。）";
    page51.choices.push_back({"前往青石镇", -2, "town"});
    pages.insert(51, page51);

    screenControl->setStories(pages);
}

void cave::resetStory()
{
    screenControl->resetStory();
}

void cave::onReturnToParent()
{
    emit Return();
    screenTransition->hideScreen(this);
}

void cave::onSceneActionRequested(const QString& action)
{
    screenTransition->hideScreen(this);
    emit navigateTo(action);
}

void cave::on_button_back_clicked(bool checked)
{
    Q_UNUSED(checked);
    emit Return();
    screenTransition->hideScreen(this);
}

void cave::on_button_next_clicked(bool checked)
{
    Q_UNUSED(checked);
    travel *dlg = new travel(this);
    connect(dlg, &travel::destinationSelected, this, &cave::navigateTo);
    connect(dlg, &travel::destinationSelected, dlg, &QDialog::deleteLater);
    dlg->exec();
}

void cave::on_button3_clicked(bool checked) { Q_UNUSED(checked); int c=screenControl->getCurrentPage(); if(c==1) { track("bamian_choice",11); ui->button_next->setVisible(true); } if(c==13) track("warrior_won",true); screenControl->onChoiceClicked(0); if(c==0||screenControl->getCurrentPage()==0) refreshHub(); }
void cave::on_button4_clicked(bool checked) { Q_UNUSED(checked); int c=screenControl->getCurrentPage(); if(c==1) { track("bamian_choice",12); ui->button_next->setVisible(true); } if(c==4) track("mage_kneel",true); screenControl->onChoiceClicked(1); if(c==0||screenControl->getCurrentPage()==0) refreshHub(); }
void cave::on_button5_clicked(bool checked) { Q_UNUSED(checked); int c=screenControl->getCurrentPage(); if(c==1) track("bamian_choice",13); if(c==4) track("mage_nokneel",true); screenControl->onChoiceClicked(2); if(c==0||screenControl->getCurrentPage()==0) refreshHub(); }
void cave::on_button6_clicked(bool checked) { Q_UNUSED(checked); int c=screenControl->getCurrentPage(); if(c==1) track("bamian_choice",14); screenControl->onChoiceClicked(3); if(c==0||screenControl->getCurrentPage()==0) refreshHub(); }
void cave::on_button7_clicked(bool checked) { Q_UNUSED(checked); int c=screenControl->getCurrentPage(); if(c==1) track("bamian_choice",15); screenControl->onChoiceClicked(4); if(c==0||screenControl->getCurrentPage()==0) refreshHub(); }
void cave::track(const QString& k, const QVariant& v) { m_state[k]=v; }
QVariant cave::st(const QString& k) const { return m_state.value(k); }
void cave::refreshHub() { if(!pages.contains(0)) return; StoryPage& p=pages[0]; p.choices.clear(); p.choices.push_back({"\"我来见疤面。\"", 1}); if(st("bamian_choice").toInt()>0) p.choices.push_back({"（已见过疤面）深入洞窟", 5}); p.choices.push_back({"（不废话，直接闯进去）", 2}); if(playerClass=="盗贼") p.choices.push_back({"（盗贼专属）从侧面的通风口溜进去", 3}); if(playerClass=="法师") p.choices.push_back({"（法师专属）洞内有元力波动——先去岔路看看", 4}); p.choices.push_back({"返回密林", -2, "forest"}); if(st("bamian_choice").toInt()==11||st("bamian_choice").toInt()==13||st("bamian_choice").toInt()==14) p.choices.push_back({"护送瑶儿去青石镇", -2, "town"}); }
