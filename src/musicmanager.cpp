#include "musicmanager.h"
#include <QWidget>

MusicManager* MusicManager::s_instance = nullptr;

MusicManager* MusicManager::instance()
{
    if (!s_instance) {
        s_instance = new MusicManager();
    }
    return s_instance;
}

MusicManager::MusicManager(QObject *parent)
    : QObject(parent)
{
}

QString MusicManager::resourcePath(const QString& sceneName) const
{
    static QMap<QString, QString> paths = {
        {"init",     "qrc:/audio/init_background.mp3"},
        {"character","qrc:/audio/character_background.mp3"},
        {"village",  "qrc:/audio/village_background.mp3"},
        {"forest",   "qrc:/audio/forest_background.mp3"},
        {"cave",     "qrc:/audio/cave_background.mp3"},
        {"town",     "qrc:/audio/town_background.mp3"},
        {"city",     "qrc:/audio/city_background.mp3"},
    };
    return paths.value(sceneName, "");
}

void MusicManager::play(QWidget *parent, const QString& sceneName)
{
    if (!parent) return;

    QString path = resourcePath(sceneName);
    if (path.isEmpty()) return;

    QMediaPlayer *bgm = new QMediaPlayer(parent);
    QAudioOutput *audio = new QAudioOutput(parent);
    bgm->setAudioOutput(audio);
    bgm->setSource(QUrl(path));
    bgm->setLoops(QMediaPlayer::Infinite);
    bgm->play();
}

void MusicManager::stop(QWidget *widget)
{
    if (!widget) return;
    QList<QMediaPlayer*> players = widget->findChildren<QMediaPlayer*>();
    for (QMediaPlayer* p : players) {
        p->stop();
    }
}

void MusicManager::stopAll(QWidget *widget)
{
    stop(widget);
}
