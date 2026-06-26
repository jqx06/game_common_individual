QT += widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/cave.cpp \
    src/city.cpp \
    src/forest.cpp \
    src/character.cpp \
    src/function.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/musicmanager.cpp \
    src/rule.cpp \
    src/savemanager.cpp \
    src/secondwindow.cpp \
    src/town.cpp \
    src/travel.cpp \
    src/village.cpp

HEADERS += \
    include/cave.h \
    include/city.h \
    include/forest.h \
    include/character.h \
    include/function.h \
    include/mainwindow.h \
    include/musicmanager.h \
    include/rule.h \
    include/savemanager.h \
    include/secondwindow.h \
    include/town.h \
    include/travel.h \
    include/village.h

FORMS += \
    ui/cave.ui \
    ui/character.ui \
    ui/city.ui \
    ui/forest.ui \
    ui/mainwindow.ui \
    ui/rule.ui \
    ui/secondwindow.ui \
    ui/town.ui \
    ui/travel.ui \
    ui/village.ui

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
