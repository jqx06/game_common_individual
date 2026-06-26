#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMap>

class MusicManager : public QObject
{
    Q_OBJECT

public:
    static MusicManager* instance();

    void play(QWidget *parent, const QString& sceneName);
    void stop(QWidget *widget);
    void stopAll(QWidget *widget);

private:
    explicit MusicManager(QObject *parent = nullptr);
    QString resourcePath(const QString& sceneName) const;

    static MusicManager *s_instance;
};

#endif // MUSICMANAGER_H
