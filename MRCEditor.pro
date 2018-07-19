#-------------------------------------------------
#
# Project created by QtCreator 2017-11-24T10:00:03
#
#-------------------------------------------------

QT       += core gui charts opengl

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
    volume/BasicControlWidget.cpp \
    volume/BMPWriter.cpp \
    volume/ClassificationWidget.cpp \
    volume/DoubleSlider.cpp \
    volume/framebufferObject.cpp \
    volume/TF1DEditor.cpp \
    volume/TF1DMappingCanvas.cpp \
    volume/TF1DTextureCanvas.cpp \
    volume/TransferFunction1DWidget.cpp \
    volume/Volume.cpp \
    volume/VolumeRenderWidget.cpp \
    volume/Rendering/Shader.cpp \
    volume/Rendering/ShaderProgram.cpp \
    volume/VolumeRenderingWindow.cpp \
    volume/TF2DEditor.cpp \
    volume/TF2DMappingCanvas.cpp \
    volume/TF2DPrimitive.cpp \
    volume/Healpix/cxxutils.cpp \
    volume/Healpix/healpix_base.cpp \
    markinfowidget.cpp \
    volumewidget.cpp \
    volume/renderview.cpp \
    volume/camera.cpp


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
    volume/BasicControlWidget.h \
    volume/BMPWriter.h \
    volume/ClassificationWidget.h \
    volume/DoubleSlider.h \
    volume/framebufferObject.h \
    volume/ModelData.h \
    volume/ShaderData.h \
    volume/TF1DEditor.h \
    volume/TF1DMappingCanvas.h \
    volume/TF1DMappingKey.h \
    volume/TF1DTextureCanvas.h \
    volume/TransferFunction1DWidget.h \
    volume/Volume.h \
    volume/VolumeRenderWidget.h \
    volume/Rendering/matrix2.h \
    volume/Rendering/matrix3.h \
    volume/Rendering/matrix4.h \
    volume/Rendering/MIPRender.h \
    volume/Rendering/Shader.h \
    volume/Rendering/ShaderProgram.h \
    volume/Rendering/Vector.h \
    volume/Rendering/VolumeLightingRenderingTF2D.h \
    volume/Rendering/VolumeLightingRenderTF1D.h \
    volume/Rendering/VolumeRenderingTF1D.h \
    volume/Healpix/arr.h \
    volume/Healpix/constants.h \
    volume/Healpix/cxxutils.h \
    volume/Healpix/healpix_base.h \
    volume/Healpix/message_error.h \
    volume/Healpix/pointing.h \
    volume/Healpix/vec3.h \
    volume/VolumeRenderingWindow.h \
    volume/TF2DEditor.h \
    volume/TF2DPrimitive.h \
    volume/TF2DMappingCanvas.h \
    markinfowidget.h \
    volumewidget.h \
    volume/renderview.h \
    volume/camera.h


FORMS    +=

CONFIG += c++11

RESOURCES += \
    resources.qrc



INCLUDEPATH += $$PWD/volume/GL/include
DEPENDPATH += $$PWD/volume/GL/include



INCLUDEPATH += $$PWD/volume/GL/include
DEPENDPATH += $$PWD/volume/GL/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/volume/GL/lib/Release/x64/ -lglew32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/volume/GL/lib/Release/x64/ -lglew32d
else:unix: LIBS += -L$$PWD/volume/GL/lib/Release/x64/ -lglew32

INCLUDEPATH += $$PWD/volume/GL/include
DEPENDPATH += $$PWD/volume/GL/include
