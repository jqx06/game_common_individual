#include "village.h"
#include "ui_village.h"
#include "travel.h"
#include "musicmanager.h"
#include <QPalette>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

village::village(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::village)
    , screenTransition(new ScreenTransition(this))
    , screenControl(new ScreenControl(this))
    , playerClass("")
{
    ui->setupUi(this);

    QPixmap pixmap(":/image/asset/village_background.png");
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    MusicManager::instance()->play(this, "village");

    //文字投影（渲染）解决文字看不清楚的问题
    QGraphicsDropShadowEffect *ts = new QGraphicsDropShadowEffect();
    ts->setBlurRadius(6);
    ts->setOffset(3, 3);
    ts->setColor(QColor(0, 0, 0, 180));
    ui->textEdit->setGraphicsEffect(ts);

    screenControl->setupTextEdit(ui->textEdit);
    screenControl->setButtons(ui->button3, ui->button4, ui->button5, ui->button6, ui->button7);

    ui->button_next->setVisible(false);
    ui->button_next->setText("移动");
    ui->button_back->setVisible(true);

    connect(screenControl, &ScreenControl::returnToParent, this, &village::onReturnToParent);
    connect(screenControl, &ScreenControl::sceneActionRequested, this, &village::onSceneActionRequested);

    // 直接构建完整故事
    rebuildStory();
    screenControl->resetStory();
}

village::~village()
{
    delete ui;
}

void village::setPlayerClass(const QString& className)
{
    playerClass = className;
    rebuildStory();
    screenControl->resetStory();
}

void village::rebuildStory()
{
    pages.clear();

    StoryPage page0;
    page0.text = "晨光洒在青石板铺就的长街上，炊烟从错落的屋檐间升起。\n\n"
                 "这里是桃源村——南柯南境最不起眼的边陲小村。\n\n"
                 "街角的早餐摊冒着热气，老板手脚麻利地翻着油条；\n"
                 "铁匠铺传来叮叮当当的敲打声，节奏沉稳，像村子的心跳；\n"
                 "药铺门口晒着一排草药，苦香混在晨风里，飘过半条街。\n\n"
                 "一切看起来平和安详——\n"
                 "只要你不去看村口那张被撕了一半的告示：\n\n"
                 "「近日黑风洞山贼猖獗，过往商旅多加小心。\n"
                 "  若有义士愿助剿贼，桃源村上下感激不尽。\n"
                 "  ——村长·覃伯」\n\n"
                 "风把残破的告示吹得猎猎作响。\n"
                 "你的旅程，将从这里开始。\n\n"
                 "村口老槐树下坐着一位摇蒲扇的老者，铁匠铺锤声正响，\n"
                 "药铺门口有人在晒药，树下还有个穿灰斗篷的身影。";
    page0.choices.push_back({"与村长·覃伯对话", 1});
    page0.choices.push_back({"与铁匠·庄叔对话", 2});
    page0.choices.push_back({"与药铺·田姨对话", 3});
    page0.choices.push_back({"与神秘旅人对话", 4});
    page0.choices.push_back({"前往幽暗密林", -2, "forest"});
    pages.insert(0, page0);

    StoryPage page1;
    if (playerClass == "战士") {
        page1.text = "一位七十出头的老者坐在老槐树下，满头白发却双目有神。\n"
                     "他瞥了一眼你腰间的兵器——目光在你握刀的手上停了一瞬。\n"
                     "那是一个曾经握过刀的人才会有的眼神。\n\n"
                     "\"练家子。\"他点了点头，不是疑问，是确认。\n"
                     "\"老夫年轻时也在山里跟野兽搏过命。你手上的茧——\n"
                     "不是种地的茧，是握刀的茧。\"\n\n"
                     "他摇了摇蒲扇。\n"
                     "\"黑风洞那伙山贼劫了我们三趟粮车。村里能打的都跑了。\n"
                     "你这种人——是来帮忙的，还是来找架打的？\"";
        page1.choices.push_back({"\"帮忙。带我去黑风洞。\"", 11});
        page1.choices.push_back({"\"先看看村里情况再说。\"", 12});
        page1.choices.push_back({"\"（握紧刀柄，没有回答）\"", 13});
        page1.choices.push_back({"\"你手上的旧伤——怎么来的？\"", 14});
    } else if (playerClass == "刺客") {
        page1.text = "一位七十出头的老者坐在老槐树下。他本来在摇蒲扇——\n"
                     "但你走进他十步之内的时候，他的手停了。\n\n"
                     "不是你发出了声音。恰恰相反——你太安静了。\n"
                     "安静到一个在村口坐了三十年、听着各路人马脚步声的老人，\n"
                     "忽然听到了一个——没有脚步声的人。\n\n"
                     "\"……稀客。\"他抬起头，老眼在你身上扫了一遍。\n"
                     "\"你这种人老夫见过。不多。但每一个——都不是路过。\"\n\n"
                     "他重新摇起蒲扇，但节奏变了。变慢了。\n"
                     "\"说吧。你是来找人的，还是来杀人的？\"";
        page1.choices.push_back({"\"找人。黑风洞抓了一个姑娘。\"", 11});
        page1.choices.push_back({"\"……谁都不是。只是路过。\"", 12});
        page1.choices.push_back({"（沉默。让眼神替他回答。）", 13});
        page1.choices.push_back({"\"你怎么发现我的？\"", 14});
    } else if (playerClass == "法师") {
        page1.text = "一位七十出头的老者坐在老槐树下。你还没走近——\n"
                     "他手里的蒲扇就停了。\n\n"
                     "不是因为你的脚步声。是因为你身上散发的元力波动——\n"
                     "对于普通人来说不可察觉，但对于一个在元力浸染的土地上\n"
                     "活了七十年的人，那就像黑夜里的烛火一样明显。\n\n"
                     "\"除咎师。\"他用了这三个字。不是疑问。\n"
                     "\"老夫上一次见到除咎师——是八年前。\n"
                     "那人穿着白袍，在村口站了一炷香就走了。\n"
                     "走后第三天，村东头的井水变苦了。\"\n\n"
                     "他的眼神在你身上停住。\n"
                     "\"你是来找东西的。老夫闻得出来——你在找一种已经绝迹了很久的东西。\"";
        page1.choices.push_back({"\"龙骨。我在找龙骨。\"", 11});
        page1.choices.push_back({"\"我只是路过。不打扰村民。\"", 12});
        page1.choices.push_back({"（展开谛观——扫描整个村子）", 13});
        page1.choices.push_back({"\"八年前那个人——是不是叫青玉案？\"", 14});
    } else if (playerClass == "富商") {
        page1.text = "一位七十出头的老者坐在老槐树下。他先是看到了你的脸——\n"
                     "然后看到了你的衣料，你的鞋，你腰间那个鼓鼓的钱袋。\n\n"
                     "他的眉毛挑了一下。那是生意人在打量另一个生意人的眼神。\n\n"
                     "\"绸缎料子。青石镇才有的货。\"他点了点头。\n"
                     "\"你是来做生意的？这穷乡僻壤——除了粮食，没什么好卖的。\n"
                     "除非——你是来买情报的。\"\n\n"
                     "他摇了摇蒲扇，不紧不慢。\n"
                     "\"情报这东西，比粮食贵。但比人命便宜。\n"
                     "你想知道黑风洞的事——老夫可以告诉你。价钱嘛——\"\n"
                     "他顿了顿，\"先说你想要什么。\"";
        page1.choices.push_back({"\"我要黑风洞的地图和人质情报。开价。\"", 11});
        page1.choices.push_back({"\"我不是来做买卖的。只是路过。\"", 12});
        page1.choices.push_back({"（把一枚金币放在石头上）\"先付定金。\"", 13});
        page1.choices.push_back({"\"老人家，这村子值多少钱？\"", 14});
    } else if (playerClass == "盗贼") {
        page1.text = "一位七十出头的老者坐在老槐树下。你还没开口——\n"
                     "他的目光已经在你身上扫了两个来回。\n"
                     "然后他笑了。是一种看穿了什么但不说破的笑。\n\n"
                     "\"你的手——刚才在村口那个货摊旁边停了一下。\n"
                     "老夫看见了。你没偷东西。但你差一点就偷了。\"\n\n"
                     "他摇了摇蒲扇，扇出来的风把你额前的头发吹起来。\n"
                     "\"别紧张。这村里没什么好偷的。但黑风洞里——\n"
                     "据说疤面藏了一批从青石镇抢来的货。\n"
                     "你要是能顺手'取'回来——老夫当没看见。\"\n\n"
                     "他眨了眨眼。那是一个老人的狡黠，不是恶意。";
        page1.choices.push_back({"\"我没偷——但我可以帮你'取'回来。\"", 11});
        page1.choices.push_back({"\"我只是路过。不惹麻烦。\"", 12});
        page1.choices.push_back({"（狡黠一笑）\"你眼力不错。\"", 13});
        page1.choices.push_back({"\"你到底是什么人？\"", 14});
    }
    pages.insert(1, page1);

    // 覃伯分支 14：了解村子
    StoryPage page14;
    page14.text = "覃伯缓缓摇着蒲扇，目光落在远处苍岭山脉的轮廓上。\n\n"
                  "\"这个村子，叫桃源。名字好听，但不是什么世外桃源。\n"
                  "三十年前这里还是一片荒地。是一群逃难的人——\n"
                  "包括我，包括铁匠庄叔，包括药铺田丫头——\n"
                  "一点一点开出田，修起路，搭起房子。\"\n\n"
                  "他停了停，蒲扇指着村口。\n"
                  "\"你看到的那棵老槐树，是第一批来的人种的。\n"
                  "种树的人后来被山里的野兽咬死了。树活到了现在。\"\n\n"
                  "他的声音很平，像在说一件与自己无关的事。";
    page14.choices.push_back({"\"所以这村子对你意味着什么？\"", 141});
    page14.choices.push_back({"回到村口", 0});
    pages.insert(14, page14);

    StoryPage page141;
    page141.text = "覃伯沉默了很久。久到你以为他不会回答了。\n\n"
                   "然后他说了四个字。\n\n"
                   "\"我的命。\"\n\n"
                   "蒲扇重新摇了起来，不快不慢。\n"
                   "\"你若真有心——先去密林看看。回来之后，\n"
                   "不管你是带回了山贼的刀，还是带回了自己的命，\n"
                   "老夫都在这儿等你。\"";
    page141.choices.push_back({"前往密林", -2, "forest"});
    page141.choices.push_back({"回到村口", 0});
    pages.insert(141, page141);

    // 覃伯分支 11：接受委托
    StoryPage page11;
    page11.text = "覃伯眼睛一亮，身子微微前倾。\n\n"
                  "\"好！有胆色。实不相瞒，黑风洞那伙山贼已经劫了我们三趟粮车了。\n"
                  "村里能打的年轻人要么跑了，要么伤了。你若愿意出手，村民自当厚报。\"\n\n"
                  "他从怀里摸出一张用油纸包好的地图，地图上有密林到黑风洞的标注。\n"
                  "\"拿着。这是我年轻时候画的——那时候山里还没山贼，只有野猪。\"";
    page11.choices.push_back({"收下地图，前往密林", -2, "forest"});
    page11.choices.push_back({"先逛逛村里", 0});
    pages.insert(11, page11);

    // 覃伯分支 12：路过
    StoryPage page12;
    page12.text = "覃伯也不介意，蒲扇不紧不慢地摇着，只是叹了口气。\n\n"
                  "\"理解，乱世自保是人之常情。不过往北走就是密林，山贼常在那出没，\n"
                  "你一个人走夜路——多加小心。\"\n\n"
                  "他顿了顿，补了一句：\n"
                  "\"要是改了主意，老朽每天都在这里。\"";
    page12.choices.push_back({"回到村口", 0});
    page12.choices.push_back({"前往幽暗密林", -2, "forest"});
    pages.insert(12, page12);

    // 覃伯分支 13：微笑不语
    StoryPage page13;
    page13.text = "覃伯打量你片刻，忽然笑了。笑完之后摇了摇头，像是对自己说了一句什么——你没听清。\n\n"
                  "\"不说话？有意思。老夫活了七十年，最怕的不是凶神恶煞之辈，\n"
                  "而是那些不说话的——看不透，才最叫人忌惮。\"\n\n"
                  "他从椅子上起身，往你手里塞了一包干粮。\n"
                  "\"拿着。不管你心里打什么算盘——别饿着肚子就行。\"";
    page13.choices.push_back({"回到村口", 0});
    page13.choices.push_back({"前往幽暗密林", -2, "forest"});
    pages.insert(13, page13);

    // 第 2 页：铁匠·庄叔

    StoryPage page2;
    if (playerClass == "战士") {
        page2.text = "四十五岁上下的中年壮汉，右臂深蓝色盘龙纹身。赤着上身抡锤，\n"
                     "汗水沿着背脊滑落，蒸汽在铁锤落下时嘶嘶作响。\n\n"
                     "他头也不抬。但你站在门口的时候——\n"
                     "他的锤声变了。不是随机的变化——是他在用锤声试探你。\n"
                     "重一锤，轻一锤，然后停了。\n\n"
                     "\"你练过。\"他放下铁锤，第一次正眼看你。\n"
                     "\"不是花架子——你身上有实战的味道。\n"
                     "刀还是拳？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"都有。但我更相信手里这把。\"", 21});
        c2.push_back({"\"你手臂上的纹身——青石镇守军的标记？\"", 22});
        c2.push_back({"\"（战士专属）你这一锤的力道，收了三成。\"", 23});
        c2.push_back({"\"你认识覃伯很久了？\"", 24});
        c2.push_back({"回到村口", 0});
        page2.choices = c2;
    } else if (playerClass == "刺客") {
        page2.text = "四十五岁上下的中年壮汉，赤着上身抡锤。\n"
                     "你没有走进铁匠铺。你站在门口——刚好在他余光看不到的角度。\n\n"
                     "但他还是停下了锤子。\n"
                     "不是听到了你的脚步声——是感觉到了。\n"
                     "一种在战场上磨出来的本能：有人在暗处看你。\n\n"
                     "\"出来。\"他头也不回，但声音像铁一样硬。\n"
                     "\"我在青石镇当了十年兵。躲在暗处的人——\n"
                     "要么是来杀人的，要么是来问价的。你是哪一种？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"问价。我想要一把不反光的匕首。\"", 21});
        c2.push_back({"\"你的警觉性——比活着的守卫队长还高。\"", 22});
        c2.push_back({"\"（从阴影中走出）\"两种都不是。\"", 23});
        c2.push_back({"回到村口", 0});
        page2.choices = c2;
    } else if (playerClass == "法师") {
        page2.text = "四十五岁上下的中年壮汉，右臂有深蓝色盘龙纹身。\n"
                     "你在他的铁匠铺门口停住了——不是因为锤声，\n"
                     "是因为你感应到了：他铁砧上那块铁胚里，有元力残留。\n\n"
                     "不是他注入的。是那块铁本身带着的。\n"
                     "来自苍岭山深处的矿石，被元力浸染了几百年。\n\n"
                     "\"你盯着我的铁胚看了很久了。\"他没抬头，继续抡锤。\n"
                     "\"这块铁不一样——我知道。它打出来的刀比普通的快。\n"
                     "但没人知道为什么。你知道？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"这铁里有元力。来自苍岭山深处。\"", 21});
        c2.push_back({"\"你手臂上的纹身——不是装饰。是守军的标记。\"", 22});
        c2.push_back({"\"（法师专属）我可以教你怎么提炼元力铁。\"", 23});
        c2.push_back({"回到村口", 0});
        page2.choices = c2;
    } else if (playerClass == "富商") {
        page2.text = "四十五岁上下的中年壮汉，赤着上身抡锤。\n"
                     "他的铁匠铺不大，但架子上摆的每一件兵器都是好货。\n"
                     "你扫了一眼就看出来了——这不是普通铁匠铺的水平。\n\n"
                     "\"你看了三秒。普通客人看一秒。\"\n"
                     "他放下铁锤，拿毛巾擦了把手。\n"
                     "\"你看的不是价格——你看的是货。你是做生意的。\"\n\n"
                     "他从架子上取下一把精铁剑。\n"
                     "\"这把剑在青石镇能卖三倍价钱。但我不往镇上送——\n"
                     "因为每送一次，山贼就劫一次。你有办法？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"我可以帮你把货运出去。抽两成。\"", 21});
        c2.push_back({"\"你的手艺——不该困在这个小村子里。\"", 22});
        c2.push_back({"\"先看看货。最好的那把给我。\"", 23});
        c2.push_back({"回到村口", 0});
        page2.choices = c2;
    } else if (playerClass == "盗贼") {
        page2.text = "四十五岁上下的中年壮汉，抡锤的节奏像心跳一样稳。\n"
                     "你趁他转身拿铁胚的功夫，扫了一眼柜台上的账本。\n"
                     "账记得很仔细——但有一些奇怪的符号，不是数字。\n\n"
                     "他回过头，正好看到你在看账本。\n\n"
                     "\"那不是账。\"他放下铁锤。\n"
                     "\"那是二十年前我还在守军队的时候，跟兄弟们用的暗号。\n"
                     "用来标记山贼的窝点。\"\n\n"
                     "他的眼神在你身上停了一瞬。\n"
                     "\"你翻账本的手法很熟练。练过？\"";
        QVector<StoryChoice> c2;
        c2.push_back({"\"练过。但没恶意——纯粹好奇。\"", 21});
        c2.push_back({"\"守军队的暗号——教教我？\"", 22});
        c2.push_back({"\"（盗贼专属）你这铺子后门锁不行。三秒就能开。\"", 23});
        c2.push_back({"回到村口", 0});
        page2.choices = c2;
    }
    pages.insert(2, page2);

    // 庄叔分支 24：打听覃伯

    StoryPage page24;
    page24.text = "庄叔的锤子顿了一下，然后笑了——这是你第一次看到他笑。\n\n"
                  "\"覃伯？他是我见过最倔的老头。\n"
                  "三十年前他一个人扛着一袋种子翻过苍岭山，\n"
                  "说要在这里种出一片桃林。\"\n\n"
                  "他指了指村口的方向。\n"
                  "\"你看到那棵老槐树没有？那不是桃树。桃树没种活。\n"
                  "但他在这棵槐树下坐了三十年。\n"
                  "他说——没种出桃子不要紧，能看着别人吃上饭就行。\"";
    page24.choices.push_back({"前往密林", -2, "forest"});
    page24.choices.push_back({"回到村口", 0});
    pages.insert(24, page24);

    // 庄叔分支 21：看货

    StoryPage page21;
    page21.text = "庄叔终于放下了铁锤，拿毛巾擦了把手。\n\n"
                  "\"好货不多，但够用。\"\n\n"
                  "他从架子上取下几把兵器，在炉火映照下，刀身泛着内敛的光。\n"
                  "一把短刀，一把精铁剑，还有一面沉甸甸的铁盾。\n"
                  "不花哨，但扎实——跟锻造它们的人一样。\n\n"
                  "他拍了拍那面铁盾：\n"
                  "\"这盾挡过十七刀。每一刀的印子都在。\n"
                  "你要是走密林那条路——带上它，多一条命。\"";
    page21.choices.push_back({"挑选短刀（轻便，适合快攻）", 211});
    page21.choices.push_back({"挑选精铁剑（平衡，攻守兼备）", 212});
    page21.choices.push_back({"\"我只是看看，不买。\"", 213});
    page21.choices.push_back({"回到村口", 0});
    pages.insert(21, page21);

    StoryPage page211;
    page211.text = "庄叔拿起短刀，在指尖试了试刃口。\n\n"
                   "\"这把刀的前任主人是个猎户——老赵。\n"
                   "你应该会在密林碰到他。\n"
                   "他用这把刀剥过十七张狼皮。后来换了长猎刀，\n"
                   "这把就放我这里寄卖。\"\n\n"
                   "他把刀递给你。刀柄被磨得光滑发亮——被握了无数次。\n"
                   "\"拿着。活着回来再给钱。\"";
    page211.choices.push_back({"收下短刀，前往密林", -2, "forest"});
    page211.choices.push_back({"回到村口", 0});
    pages.insert(211, page211);

    StoryPage page212;
    page212.text = "庄叔从架子的最上层取下一个长布包，一层一层地拆开。\n\n"
                   "剑身上有一道天然的水纹，像是被封印在铁里的溪流。\n\n"
                   "\"这把剑我锻了三天。本来想自己留着——\n"
                   "但好剑跟好刀一样，不用就是废铁。\"";
    page212.choices.push_back({"收下精铁剑，前往密林", -2, "forest"});
    page212.choices.push_back({"回到村口", 0});
    pages.insert(212, page212);

    StoryPage page213;
    page213.text = "庄叔也不恼，重新抄起铁锤。\n\n"
                   "\"不买也无所谓。看一眼不要钱。\n"
                   "不过给你句忠告——密林里那些山贼用的刀虽然粗糙，\n"
                   "但他们的刀法是死人堆里练出来的。\n"
                   "你手上要是什么都没有，至少带根结实点的木棍。\"\n\n"
                   "他说到'死人堆'的时候，握锤的手紧了一下。";
    page213.choices.push_back({"\"你去过黑风洞？\"", 2131});
    page213.choices.push_back({"回到村口", 0});
    pages.insert(213, page213);

    StoryPage page2131;
    page2131.text = "庄叔的锤子停了一拍。只有一拍。然后继续抡。\n\n"
                    "\"去过。二十年前。那时候洞里还没有山贼——\n"
                    "只有一群饿疯了的难民。\"\n\n"
                    "他没有继续说下去。炉火烧得更旺了，映红了他半张脸。\n"
                    "你看到他右臂的盘龙纹身在火光中像是在动。\n\n"
                    "\"你要是去——别走最深的岔路。\n"
                    "那里的石壁后面，埋着不该被挖出来的东西。\"";
    page2131.choices.push_back({"前往密林", -2, "forest"});
    page2131.choices.push_back({"回到村口", 0});
    pages.insert(2131, page2131);

    // 庄叔分支 22：纹身往事

    StoryPage page22;
    page22.text = "庄叔的动作停了一瞬。他低头看了眼自己手臂上的盘龙纹身，眼神暗了暗。\n\n"
                  "\"……那是二十年前的事了。别问，问就是没了兄弟，留下条疤。\"\n\n"
                  "他转过身去，重新抡起铁锤。但你注意到——锤声比刚才重了几分，\n"
                  "节奏也不再那么稳了。\n\n"
                  "过了很久，他又补了一句，声音闷在炉火的轰鸣里：\n"
                  "\"青石镇当年的守军队，一共三十七个人。\n"
                  "活到现在的——只剩我和另一个。\"";
    page22.choices.push_back({"\"另一个是谁？\"", 221});
    page22.choices.push_back({"不再追问，回到村口", 0});
    pages.insert(22, page22);

    StoryPage page221;
    page221.text = "庄叔把铁锤放在铁砧上。锤声停了。\n\n"
                   "\"青石镇的守卫队长——铁峰。\n"
                   "你要是路过青石镇，替我跟他说一声：\n"
                   "庄老三还没死。还在打铁。\"\n\n"
                   "他重新举起铁锤。这一次，锤声稳了。\n"
                   "和你的心跳一样稳。";
    page221.choices.push_back({"\"我会转告他的。\"", 0});
    page221.choices.push_back({"前往密林", -2, "forest"});
    pages.insert(221, page221);

    // 庄叔分支 23：战士专属

    StoryPage page23;
    page23.text = "庄叔猛地抬头，第一次正眼打量你。\n\n"
                  "\"……你眼力不错。这收的三成力道，是因为怕锤下的铁胚吃不住。\n"
                  "看得出这一点的，整个桃源村不超过三个人。\"\n\n"
                  "他放下锤子，从炉子后面摸出一把被旧布层层包裹的长刀。\n"
                  "解开布条时，他的手在发抖——不是年老，是舍不得。\n\n"
                  "\"这把刀跟了我半辈子。现在我老了，它也该换个主人。\"\n\n"
                  "他顿了顿，又道：\n"
                  "\"只有一条——别让这刀沾无辜人的血。能做到吗？\"";
    page23.choices.push_back({"\"能。\"（获得庄叔的旧刀）", 0});
    page23.choices.push_back({"\"我不能保证。\"", 0});
    pages.insert(23, page23);

    // 第 3 页：药铺·田姨

    StoryPage page3;
    if (playerClass == "战士") {
        page3.text = "五十出头的妇人，手脚麻利地整理柜台。\n"
                     "她看了你一眼——准确说，看了你身上的伤。\n\n"
                     "\"肩膀上的淤青。三天前的新伤。还有手腕——扭过。\"\n"
                     "她从柜台后面绕出来，把一碗暗褐色的药汤推到你面前。\n"
                     "\"喝。不收钱。你要去黑风洞——对吧？\n"
                     "去之前把身上的旧伤养好。到了那边，没人给你熬药。\"\n\n"
                     "她说话的语气不像在建议——像在命令。\n"
                     "这大概是她表达关心的方式。";
        QVector<StoryChoice> c3;
        c3.push_back({"\"多谢。\"（喝下）", 31});
        c3.push_back({"\"我不需要。\"", 32});
        c3.push_back({"\"你怎么知道我要去黑风洞？\"", 34});
        c3.push_back({"回到村口", 0});
        page3.choices = c3;
    } else if (playerClass == "刺客") {
        page3.text = "五十出头的妇人，手脚麻利地收拾药材。\n"
                     "你走进药铺的时候，没有任何声音。\n"
                     "但她还是在你踏进门槛的瞬间转过了身。\n\n"
                     "\"你的脚步声比猫还轻。\"她把一碗药放在柜台上。\n"
                     "\"但你的血——我闻得到。你的血很冷。不是身体的冷——\n"
                     "是某种……压抑了很久的东西。\"\n\n"
                     "她盯着你看了两秒。\n"
                     "\"我不问你是做什么的。但你要是受伤了——\n"
                     "别忍着。到我这里来。刀伤药我比任何郎中都熟练。\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"谢谢。\"（接过药）", 31});
        c3.push_back({"\"我不是你以为的那种人。\"", 32});
        c3.push_back({"\"你怎么知道我是做什么的？\"", 34});
        c3.push_back({"回到村口", 0});
        page3.choices = c3;
    } else if (playerClass == "法师") {
        page3.text = "五十出头的妇人，手在抓药，眼睛却在你身上。\n"
                     "你踏进药铺的一瞬——她停手了。\n\n"
                     "\"茯神，当归，川芎——\"她一个一个念出药名，\n"
                     "然后停在了你面前。\n"
                     "\"——还有一味，你闻出来了吗？\"\n\n"
                     "她把熬好的药汤倒进碗里，暗褐色的药汁在碗底泛着幽微的蓝光。\n"
                     "那是龙骨入药才会有的光泽。\n\n"
                     "\"能看出这是什么的人，整个南境不超过二十个。\n"
                     "你是今天第二个。\"\n"
                     "她没说是谁第一个。但你知道——一定是她自己。";
        QVector<StoryChoice> c3;
        c3.push_back({"\"龙骨。你在药里放了龙骨。\"", 33});
        c3.push_back({"\"我只是路过——不想惹麻烦。\"", 32});
        c3.push_back({"\"第一个是谁？\"", 34});
        c3.push_back({"回到村口", 0});
        page3.choices = c3;
    } else if (playerClass == "富商") {
        page3.text = "五十出头的妇人，手脚麻利地打包药草。\n"
                     "你打量了一圈她的药铺——货很全，品相上乘，\n"
                     "但定价偏低。这不是不会做生意——是故意不赚钱。\n\n"
                     "\"你看的不是药。\"她把一碗热药放在柜台上。\n"
                     "\"你看的是价格。你是做买卖的。\"\n\n"
                     "她擦了把手，眼神里有一种商人的精明。\n"
                     "\"我这些药——最好的不是摆在台面上的。\n"
                     "最好的在柜子最里面。成本太高，卖不出去。\n"
                     "但你要是识货——我可以给你看一样东西。\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"拿来看看。只要货好，价钱好谈。\"", 31});
        c3.push_back({"\"我不做药材生意。只是路过。\"", 32});
        c3.push_back({"\"你亏本卖药——到底在等什么？\"", 34});
        c3.push_back({"回到村口", 0});
        page3.choices = c3;
    } else if (playerClass == "盗贼") {
        page3.text = "五十出头的妇人，正在分拣药材。\n"
                     "你趁她转身，扫了一眼柜台下的抽屉——没有上锁。\n"
                     "但你手还没动，她已经开口了。\n\n"
                     "\"别看了。那个抽屉里只有甘草。值钱的东西——\n"
                     "都在你够不着的地方。\"\n\n"
                     "她转过身，把一碗药放在柜台上，笑眯眯地看着你。\n"
                     "那笑容里没有敌意——只有一种'我知道你想干什么但我不在乎'的宽容。\n"
                     "\"偷药的人我见多了。但偷药救人的——我没见过。\n"
                     "你要什么，直接说。我不是开善堂的，但我分得清好坏。\"";
        QVector<StoryChoice> c3;
        c3.push_back({"\"我不是来偷的。只是习惯性地……看。\"", 31});
        c3.push_back({"\"你这家店——偷起来没意思。太穷了。\"", 32});
        c3.push_back({"\"你说得对。我需要金疮药。\"", 34});
        c3.push_back({"回到村口", 0});
        page3.choices = c3;
    }
    pages.insert(3, page3);

    // 田姨分支 34：问起庄叔

    StoryPage page34;
    page34.text = "田姨笑了一声，笑声里有股说不出的味道。\n\n"
                  "\"庄老三？他身上的伤比打的铁还多。\n"
                  "他那条盘龙纹身——不是图好看。是遮疤。\"\n\n"
                  "她叹了口气，继续摆弄药草。\n"
                  "\"二十年前青石镇守军跟山贼打了一仗。三十七个人，死了三十五个。\n"
                  "活下来的两个，一个是他，一个是铁峰。\n"
                  "从那以后他就再也没摸过刀——只打铁。他说打铁比杀人干净。\"";
    page34.choices.push_back({"前往密林", -2, "forest"});
    page34.choices.push_back({"回到村口", 0});
    pages.insert(34, page34);

    // 田姨分支 31：喝药

    StoryPage page31;
    page31.text = "汤药入喉，一股暖流从胃部蔓延到四肢。\n"
                  "不是那种猛烈的灼烧——而是绵长的、温和的热，\n"
                  "像是有人在体内点了一盏灯。\n\n"
                  "田姨看着你喝完，满意地笑了，眼角的皱纹挤在一起。\n\n"
                  "\"这才对嘛。年轻人别老逞强，身体是最大的本钱——别的都是虚的。\"\n\n"
                  "她把碗收回去，顺手在围裙上擦了把手。\n"
                  "\"这方子里有一味茯神，是安神的。还有一味当归，是养血的。\n"
                  "最要紧的一味……算了，不说也罢。\"\n"
                  "她的眼神闪烁了一下，像是在回忆什么。";
    page31.choices.push_back({"\"最要紧的是什么？\"", 311});
    page31.choices.push_back({"道谢离开", 0});
    pages.insert(31, page31);

    StoryPage page311;
    page311.text = "田姨犹豫了一下。然后摇了摇头，笑得更深了。\n\n"
                   "\"龙骨。说了你也不懂。这世上知道龙骨是什么的人——\n"
                   "一个巴掌数得过来。\"\n\n"
                   "她把碗放进水池，背对着你，声音忽然变低了。\n"
                   "\"你要是去了玄铁城——别跟一个穿白袍的法师打交道。\n"
                   "他喜欢下棋。也喜欢用活人当棋子。\"";
    page311.choices.push_back({"前往密林", -2, "forest"});
    page311.choices.push_back({"回到村口", 0});
    pages.insert(311, page311);

    // 田姨分支 32：拒绝

    StoryPage page32;
    page32.text = "田姨也不生气，只是把碗收了回去，顺手在围裙上擦了把手。\n\n"
                  "\"行，等你什么时候想喝了，铺子就在这儿。\n"
                  "反正我每天都要熬一锅——也不差这一碗。\"\n\n"
                  "她语气随意，但你注意到她把那碗药小心翼翼地放回了炉子边，没有倒掉。像是在等什么人。";
    page32.choices.push_back({"回到村口", 0});
    pages.insert(32, page32);

    // 田姨分支 33：法师专属

    StoryPage page33;
    page33.text = "田姨的手停了。这是她今天第一次停下——那双从你进门就在不停忙碌的手，就那样僵在半空中。\n\n"
                  "她盯着你看了几秒，眼神从热情变成了审视，又从审视变成了一种复杂的、说不清楚的神情。\n\n"
                  "\"……还有一味是龙骨。你说对了。\"\n\n"
                  "她沉默了一会儿，像是在犹豫什么。然后转身，从柜子最深处拿出一丸被封蜡包裹的丹药。\n"
                  "暗红色的蜡壳在阳光下微微透光。\n\n"
                  "\"浣尘丹。能闻出龙骨的人，整个南柯大陆不超过二十个——每一个，都是你想象不到的大人物。\"\n\n"
                  "她把丹药放在你手心，手指冰凉。\n"
                  "\"这东西放在我这里也是浪费。你拿着。但别问我是从哪里得来的——我不会说。\"";
    page33.choices.push_back({"回到村口", 0});
    pages.insert(33, page33);

    // 第 4 页：神秘旅人

    StoryPage page4;
    page4.text = "村口的老槐树下，不知何时多了一个人。\n"
                 "穿着灰扑扑的斗篷，看不清脸，只露出一只枯瘦的手，\n"
                 "正不紧不慢地剥着一颗橘子。橘子的清香和这人身上散发出的元力波动混合在一起——\n"
                 "一种矛盾的组合，像盛夏里吹过的一阵穿堂风。\n\n"
                 "\"站住。\"\n\n"
                 "他没抬头，只是朝你弹了一瓣橘子。\n"
                 "\"吃橘子。很甜——比你接下来要经历的事情甜多了。\"\n\n"
                 "\"然后回答我一个问题。\"\n\n"
                 "他终于抬起眼皮。斗篷的阴影下，你看不清完整的脸——\n"
                 "只有一只眼睛，和一个让人不安的微笑。\n\n"
                 "\"你是谁？\"";

    QVector<StoryChoice> choices4;
    if (playerClass == "战士") {
        choices4.push_back({"\"我是一个……不想再看到弱者被欺负的人。\"", 41});
    } else if (playerClass == "刺客") {
        choices4.push_back({"\"一个习惯站在阴影里的人。\"", 42});
    } else if (playerClass == "法师") {
        choices4.push_back({"\"一个……追求真相的人。\"", 43});
    } else if (playerClass == "富商") {
        choices4.push_back({"\"一个生意人。\"", 44});
    } else if (playerClass == "盗贼") {
        choices4.push_back({"\"一个……路过的。\"", 45});
    }
    choices4.push_back({"\"你先回答我——你是谁？\"", 46});
    choices4.push_back({"回到村口", 0});
    page4.choices = choices4;
    pages.insert(4, page4);

    // 旅人通用分支 46：反问

    StoryPage page46;
    page46.text = "旅人愣了一拍。然后笑了——不是之前那种高深莫测的笑，\n"
                  "而是一个被逗乐了的、真正觉得有趣的笑。\n\n"
                  "\"好。好。你是第一个反问我的。\"\n\n"
                  "他把剩下的半个橘子放在石头上，站起身来。\n"
                  "你终于看到了他斗篷下的脸——不年轻，也不老，\n"
                  "像是被时间遗忘了，卡在某个过不去的岁月里。\n\n"
                  "\"我叫一筒。不是英雄，不是大师——就是个到处乱逛的闲人。\n"
                  "橘子吃完了。问题你还没回答。下次见面——我再问一遍。\"\n\n"
                  "他转身，灰斗篷在风中翻了一下，人就不见了。\n"
                  "石头上只留着半个橘子。还新鲜。";
    page46.choices.push_back({"前往密林", -2, "forest"});
    page46.choices.push_back({"回到村口", 0});
    pages.insert(46, page46);

    // 第 5 页：更多选项（神秘旅人入口）

    StoryPage page5;
    page5.text = "你继续在村里走着，村口的老槐树下还有一个穿灰斗篷的人影，\n"
                 "正不紧不慢地剥着一颗橘子。";
    page5.choices.push_back({"与神秘旅人对话", 4});
    page5.choices.push_back({"回到村口", 0});
    pages.insert(5, page5);

    // 旅人分支 41：战士

    StoryPage page41;
    page41.text = "神秘旅人剥橘子的手停了。\n\n"
                  "\"好答案。但你知道吗？'不想看到'和'有能力阻止'——是两码事。\"\n\n"
                  "他忽然撕下半张橘子皮，在手中揉搓。橘子皮的汁液滴在地上，绽开一小片湿润的纹路。\n\n"
                  "\"你想成为哪一种？\"";
    page41.choices.push_back({"\"我选择有能力。\"", 411});
    page41.choices.push_back({"\"我只是个莽夫，没想那么多。\"", 412});
    pages.insert(41, page41);

    StoryPage page411;
    page411.text = "旅人点了点头，将手中剩下的一半橘子递给你。\n\n"
                   "\"记住这个味道。以后某一天——当有人告诉你'不可能'的时候——\n"
                   "想想这颗橘子。\"\n\n"
                   "你获得了被动「铁骨」：此后在战斗中受到致命伤时，有一次机会咬着牙站起来。\n\n"
                   "旅人站起身，转身走向村口，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page411.choices.push_back({"回到村口", 0});
    pages.insert(411, page411);

    StoryPage page412;
    page412.text = "旅人笑了笑，把另一半橘子也塞到你手里。\n\n"
                   "\"莽夫也有莽夫的好。至少——想得少的人，睡得香。\"\n\n"
                   "旅人站起身，转身走向村口，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page412.choices.push_back({"回到村口", 0});
    pages.insert(412, page412);

    // 旅人分支 42：刺客

    StoryPage page42;
    page42.text = "神秘旅人终于抬头了。斗篷下是一只独眼，在昏暗的天光下泛着冷光。\n\n"
                  "\"影子。有趣。你知道影子的宿命是什么吗？永远跟在光亮后面。别人的光亮——不是你的。\"\n\n"
                  "他顿了顿。\n\n"
                  "\"你想继续跟在别人后面，还是想——成为那个制造阴影的人？\"";
    page42.choices.push_back({"\"我要做后者。\"", 421});
    page42.choices.push_back({"\"影子也挺好，至少安全。\"", 422});
    pages.insert(42, page42);

    StoryPage page421;
    page421.text = "旅人从斗篷里摸出一个小瓶子，里面是黑得发亮的液体。\n\n"
                   "他拔开瓶塞，那液体竟在空气中蒸腾成雾，没有味道，没有声音。\n\n"
                   "\"影蛇的毒液。不是让你杀人——是告诉你，真正的力量在看不见的地方。\"\n\n"
                   "你获得了被动「暗夜行者」：潜行时更难被察觉。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page421.choices.push_back({"回到村口", 0});
    pages.insert(421, page421);

    StoryPage page422;
    page422.text = "\"安全？\"旅人将瓶子收了回去。\n\n"
                   "\"这世上最不安全的，就是以为自己是安全的。\"\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page422.choices.push_back({"回到村口", 0});
    pages.insert(422, page422);

    // 旅人分支 43：法师

    StoryPage page43;
    page43.text = "旅人忽然大笑三声，笑得太用力，把手里的橘子都捏烂了。\n\n"
                  "\"哈哈哈哈！这世上最难的事，恰恰是知道真相！你以为你看到了，其实你只是在别人允许你看到的范围里打转。\"\n\n"
                  "他摊开手掌，捏烂的橘子皮中有一瓣完好无损。他把它挑出来，放在你手心里。\n\n"
                  "\"我问你——这瓣橘子是真的，还是整颗橘子是真的？\"";
    page43.choices.push_back({"\"都是真的。视角不同。\"", 431});
    page43.choices.push_back({"\"整颗才是真的。碎片不是。\"", 432});
    pages.insert(43, page43);

    StoryPage page431;
    page431.text = "旅人眼中精光一闪，手在空中一抹——那瓣橘子竟化为一道金色的符咒，飘落在你手心。\n"
                   "符咒上写着两个古字——「谛」「观」。\n\n"
                   "\"谛者审也，观者察也。大多数人只学会了看——你要学会的是如何看见。\"\n\n"
                   "你获得了技能「谛观」：可在关键场景中探测一次隐藏信息。只能使用一次，使用时机至关重要。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page431.choices.push_back({"回到村口", 0});
    pages.insert(431, page431);

    StoryPage page432;
    page432.text = "旅人失望地摇了摇头，从怀里摸出几枚铜币抛给你。\n\n"
                   "\"碎片不是真相。愿你有一天能明白。\"\n\n"
                   "你获得了金币补偿。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page432.choices.push_back({"回到村口", 0});
    pages.insert(432, page432);

    // 旅人分支 44：富商

    StoryPage page44;
    page44.text = "\"生意人？\"旅人的嘴角扬起。\n\n"
                  "\"巧了，我也是。那你觉得——这世上最值钱的东西是什么？\"";
    page44.choices.push_back({"\"命。没有命，什么都没意义。\"", 441});
    page44.choices.push_back({"\"情报。知道得比别人多，就能比别人先赚到钱。\"", 442});
    pages.insert(44, page44);

    StoryPage page441;
    page441.text = "旅人从斗篷下摸出五个小瓷瓶，排在你面前。\n\n"
                   "\"金疮药。上好的——不是街上能买到的那种。好好活着，生意人要活着才能把账算完。\"\n\n"
                   "你获得了「金疮药」× 5。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page441.choices.push_back({"回到村口", 0});
    pages.insert(441, page441);

    StoryPage page442;
    page442.text = "旅人从怀里掏出一张皱巴巴的羊皮纸，上面用炭笔画着歪歪扭扭的线条。\n\n"
                   "\"黑风洞的内部岔路图。上个月我亲手画的——希望来得及，在你钻进去之前还能用。\"\n\n"
                   "你获得了「黑风洞地图」。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page442.choices.push_back({"回到村口", 0});
    pages.insert(442, page442);

    // 旅人分支 45：盗贼
    
    StoryPage page45;
    page45.text = "\"路过？\"旅人伸出手——你的钱袋不知什么时候已经在他手里了。\n\n"
                  "他掂了掂，露出一个玩味的笑，然后随手扔了回来。\n\n"
                  "\"手法太糙了。不过——有天赋。你知道你缺什么吗？不是手速——手速可以练。\n"
                  "你缺的是胆量。敢不敢玩一票大的？\"";
    page45.choices.push_back({"\"有多大？\"", 451});
    page45.choices.push_back({"\"小富即安。\"", 452});
    pages.insert(45, page45);

    StoryPage page451;
    page451.text = "旅人凑近了你，声音低得像风吹过树叶。\n\n"
                   "\"大到——你偷的不止是东西。偷情报，偷时间，偷一个活人的身份。\n"
                   "大到一旦开始，就再也停不下来。\"\n\n"
                   "他从自己斗篷上撕下一小块布料，塞进你手心。\n"
                   "布上绣了一个极小的图案——一只空着的手。\n\n"
                   "\"这个标记，在青石镇黑市——老鬼认。他看到这个，会给你开一扇别人找不到的门。\"\n\n"
                   "你获得了「黑市标记」。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page451.choices.push_back({"回到村口", 0});
    pages.insert(451, page451);

    StoryPage page452;
    page452.text = "旅人笑着摇了摇头，从怀里摸出几枚铜币。\n\n"
                   "\"小富即安？也行。这世上敢玩大的不多——敢承认自己不想玩大的，其实也不多。\"\n\n"
                   "你获得了金币 × 60。\n\n"
                   "旅人站起身，灰扑扑的斗篷很快消失在晨雾中。\n"
                   "老槐树还在那里，地上只留了一堆橘子皮。";
    page452.choices.push_back({"回到村口", 0});
    pages.insert(452, page452);

    // 设置故事数据到 ScreenControl
    screenControl->setStories(pages);
}

void village::resetStory()
{
    screenControl->resetStory();
}

void village::onReturnToParent()
{
    emit Return();
    screenTransition->hideScreen(this);
}

void village::onSceneActionRequested(const QString& action)
{
    if (action == "forest") {
        screenTransition->hideScreen(this);
        emit goToForest();
    }
}

void village::on_button_back_clicked(bool checked)
{
    Q_UNUSED(checked);
    emit Return();
    screenTransition->hideScreen(this);
}

void village::on_button_next_clicked(bool checked)
{
    Q_UNUSED(checked);

    travel *dlg = new travel(this);
    connect(dlg, &travel::destinationSelected, this, &village::navigateTo);
    connect(dlg, &travel::destinationSelected, dlg, &QDialog::deleteLater);
    dlg->exec();
}

void village::on_button3_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    // 覃伯 page1: choice0=11(接受) choice1=12(路过) choice2=13(不语) choice3=14(问村)
    if (cur == 1) { track("qinbo_choice", 11); track("visited_qinbo", true); ui->button_next->setVisible(true); }
    // 庄叔 page2: choice0=21(看货) choice1=22(纹身) choice2=23(战士) choice3=24(覃伯)
    if (cur == 2) { track("visited_zhuang", true); }
    // 田姨 page3: choice0=31(喝药) choice1=32(不用) choice2=33(法师) choice3=34(庄叔)
    if (cur == 3) { track("visited_tian", true); }
    // 战士获刀 page231
    if (cur == 23) track("got_zhuang_sword", true);
    // 法师获丹 page33
    if (cur == 33) track("got_huanchen_dan", true);
    // 富商得地图 page442
    if (cur == 442) track("got_cave_map", true);
    // 盗贼得标记 page451
    if (cur == 451) track("got_black_mark", true);
    // 旅人回答 page4: 各职业专属 → 记录
    if (cur == 4) track("traveler_answer", 1);

    screenControl->onChoiceClicked(0);
    // 如果跳回村口，刷新枢纽
    int tgt = 0;
    if (pages.contains(cur) && !pages[cur].choices.isEmpty() && pages[cur].choices[0].targetPage == 0)
        tgt = pages[cur].choices[0].targetPage;
    if (tgt == 0) refreshHub();
}

void village::on_button4_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    // 覃伯 choice1=12(路过)
    if (cur == 1) { track("qinbo_choice", 12); track("visited_qinbo", true); ui->button_next->setVisible(true); }
    if (cur == 2) track("visited_zhuang", true);
    if (cur == 3) track("visited_tian", true);
    if (cur == 4) track("traveler_answer", 2);
    screenControl->onChoiceClicked(1);
    if (cur == 1) refreshHub();
}

void village::on_button5_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    // 覃伯 choice2=13(不语)
    if (cur == 1) { track("qinbo_choice", 13); track("visited_qinbo", true); }
    if (cur == 2) track("visited_zhuang", true);
    if (cur == 3) track("visited_tian", true);
    screenControl->onChoiceClicked(2);
    if (cur == 1) refreshHub();
}

void village::on_button6_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 1) { track("qinbo_choice", 14); track("visited_qinbo", true); }
    if (cur == 2) track("visited_zhuang", true);
    if (cur == 3) track("visited_tian", true);
    screenControl->onChoiceClicked(3);
    if (cur == 1) refreshHub();
}

void village::on_button7_clicked(bool checked)
{
    Q_UNUSED(checked);
    int cur = screenControl->getCurrentPage();
    if (cur == 1) track("visited_qinbo", true);
    screenControl->onChoiceClicked(4);
}

void village::track(const QString& key, const QVariant& val)
{
    m_state[key] = val;
}

QVariant village::st(const QString& key) const
{
    return m_state.value(key);
}

void village::refreshHub()
{
    if (!pages.contains(0)) return;

    StoryPage& p0 = pages[0];
    p0.choices.clear();

    p0.choices.push_back({"与村长·覃伯对话", 1});
    p0.choices.push_back({"与铁匠·庄叔对话", 2});
    p0.choices.push_back({"与药铺·田姨对话", 3});
    p0.choices.push_back({"与神秘旅人对话", 4});

    // 已接受覃伯委托 → 解锁
    if (st("qinbo_choice").toInt() == 11) {
        p0.choices.push_back({"前往幽暗密林（已接委托）", -2, "forest"});
    }

    // 拒绝了覃伯
    if (st("qinbo_choice").toInt() == 12) {
        p0.choices.push_back({"独自前往密林", -2, "forest"});
    }

    // 见过庄叔+田姨 → 解锁隐藏
    if (st("visited_zhuang").toBool() && st("visited_tian").toBool()) {
        p0.choices.push_back({"村中的人都在帮你……该出发了", -2, "forest"});
    }

    // 战士获得了庄叔的刀
    if (playerClass == "战士" && st("got_zhuang_sword").toBool()) {
        p0.choices.push_back({"握着庄叔的旧刀，前往密林", -2, "forest"});
    }

    // 法师获得了浣尘丹
    if (playerClass == "法师" && st("got_huanchen_dan").toBool()) {
        p0.choices.push_back({"怀揣浣尘丹，前往密林", -2, "forest"});
    }

    // 富商获得了黑风洞地图
    if (playerClass == "富商" && st("got_cave_map").toBool()) {
        p0.choices.push_back({"带着黑风洞地图，前往密林", -2, "forest"});
    }

    // 盗贼获得了黑市标记
    if (playerClass == "盗贼" && st("got_black_mark").toBool()) {
        p0.choices.push_back({"揣好黑市标记，前往密林", -2, "forest"});
    }

    // 如果见过神秘旅人
    if (st("traveler_answer").toInt() > 0) {
        p0.choices.push_back({"（旅人的话还在耳边……前往密林）", -2, "forest"});
    }
}