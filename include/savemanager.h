#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QDateTime>

class SaveManager
{
public:
    SaveManager();

    // 存档操作
    bool saveGame(const QString& name,
                  const QString& playerClass,
                  const QString& currentScene);
    QJsonObject loadGame(const QString& name);
    QStringList listSaves();
    bool deleteSave(const QString& name);
    bool saveExists(const QString& name);

    // 获取存档信息
    QString getSaveInfo(const QString& name);

private:
    QString saveDir();
    QString savePath(const QString& name);
};

#endif // SAVEMANAGER_H
