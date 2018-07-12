#-------------------------------------------------
#
# Project created by QtCreator 2017-11-24T10:00:03
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MRCEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    mrc.cpp \
    histogram.cpp \
    zoomviwer.cpp \
    ItemContext.cpp \
    pixelviewer.cpp \
    titledsliderwithspinbox.cpp \
    mrcfileinfoviewer.cpp \
    imageviewer.cpp \
    profileview.cpp \
    mrcdatamodel.cpp \
    abstractplugin.cpp \
    markcategray.cpp \
    markmodel.cpp \
    abstractslicedatamodel.cpp \
    markitem.cpp \
    treeitem.cpp \
    sliceitem.cpp \
    sliceview.cpp \
    marktreeview.cpp \
    categoryitem.cpp \
    markinfowidget.cpp \
    volumewidget.cpp

HEADERS  += mainwindow.h \
    mrc.h \
    histogram.h \
    zoomviwer.h \
    ItemContext.h \
    pixelviewer.h \
    titledsliderwithspinbox.h \
    mrcfileinfoviewer.h \
    imageviewer.h \
    CImg.h \
    profileview.h \
    mrcdatamodel.h \
    abstractplugin.h \
    markmodel.h \
    globals.h \
    markcategorydialog.h \
    abstractslicedatamodel.h \
    markitem.h \
    treeitem.h \
    sliceitem.h \
    sliceview.h \
    marktreeview.h \
    categoryitem.h \
    markinfowidget.h \
    volumewidget.h

FORMS    +=

CONFIG += c++11

RESOURCES += \
    resources.qrc
