#-------------------------------------------------
#
# Project created by QtCreator 2016-06-04T12:02:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = puzzle15
TEMPLATE = app


SOURCES += main.cpp\
        puzzle15.cpp

HEADERS  += puzzle15.h

FORMS    += puzzle15.ui

# Deployment - Automatically Detect and Copy Dependencies to Build Folder
TARGET_CUSTOM_EXT = .exe
DEPLOY_COMMAND = windeployqt

CONFIG( debug, debug|release ) {
    # debug
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_CUSTOM_EXT}))
} else {
    # release
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}))
}

# Uncomment the following line to help debug the deploy command when running qmake
#warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET}
