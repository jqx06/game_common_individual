#include "savemanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

SaveManager::SaveManager()
{
    // 确保存档目录存在
    QDir dir(saveDir());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString SaveManager::saveDir()
{
    return QCoreApplication::applicationDirPath() + "/saves";
}

QString SaveManager::savePath(const QString& name)
{
    return saveDir() + "/" + name + ".json";
}

bool SaveManager::saveGame(const QString& name,
                            const QString& playerClass,
                            const QString& currentScene)
{
    QJsonObject obj;
    obj["playerClass"] = playerClass;
    obj["currentScene"] = currentScene;
    obj["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    obj["saveName"] = name;

    QJsonDocument doc(obj);
    QFile file(savePath(name));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QJsonObject SaveManager::loadGame(const QString& name)
{
    QFile file(savePath(name));
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

QStringList SaveManager::listSaves()
{
    QStringList saves;
    QDir dir(saveDir());
    QStringList files = dir.entryList({"*.json"}, QDir::Files, QDir::Time);
    for (const QString& f : files) {
        // 去掉 .json 后缀
        saves.append(f.left(f.length() - 5));
    }
    return saves;
}

bool SaveManager::deleteSave(const QString& name)
{
    QFile file(savePath(name));
    return file.remove();
}

bool SaveManager::saveExists(const QString& name)
{
    return QFile::exists(savePath(name));
}

QString SaveManager::getSaveInfo(const QString& name)
{
    QJsonObject obj = loadGame(name);
    if (obj.isEmpty()) return "";

    QString info;
    info += "职业: " + obj["playerClass"].toString() + "\n";
    info += "场景: " + obj["currentScene"].toString() + "\n";
    info += "时间: " + obj["timestamp"].toString();
    return info;
}
