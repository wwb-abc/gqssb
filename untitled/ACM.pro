QT += core gui widgets network sql charts

CONFIG += c++20

TARGET = ACM
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION=\\\"0.1.0\\\"

# 源码根目录，之后所有 include 都以 src/ 为基准
# 例如： #include "models/Enums.h"
INCLUDEPATH += $$PWD/src

SOURCES += \
    src/main.cpp \
    src/ui/MainWindow.cpp

HEADERS += \
    src/ui/MainWindow.h \
    src/models/Enums.h \
    src/models/Problem.h \
    src/models/Submission.h \
    src/models/WrongNote.h \
    src/models/ImportTask.h

FORMS += \
    src/ui/MainWindow.ui

# 构建产物集中，避免 .obj / moc 散落在源码目录里
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/out/debug
} else {
    DESTDIR = $$PWD/build/out/release
}
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR     = $$PWD/build/moc
RCC_DIR     = $$PWD/build/rcc
UI_DIR      = $$PWD/build/ui
