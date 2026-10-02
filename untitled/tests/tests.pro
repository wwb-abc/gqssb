QT += core testlib

CONFIG += c++20 console testcase
CONFIG -= app_bundle

TARGET = tst_models
TEMPLATE = app

INCLUDEPATH += $$PWD/../src

SOURCES += \
    tst_models.cpp

HEADERS += \
    ../src/models/Enums.h \
    ../src/models/Problem.h \
    ../src/models/Submission.h \
    ../src/models/WrongNote.h \
    ../src/models/ImportTask.h

# 构建产物统一放到工程根的 build/ 下，与主程序共用，便于集中清理
DESTDIR     = $$PWD/../build/out
OBJECTS_DIR = $$PWD/../build/tests_obj
MOC_DIR     = $$PWD/../build/tests_moc
