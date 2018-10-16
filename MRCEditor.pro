TEMPLATE = app
TARGET = MRCEditor
QT += core opengl gui widgets
DEPENDPATH += .

QMAKE_CXXFLAGS+= -openmp
QMAKE_LFLAGS+= -openmp


HEADERS +=./widgets/doubleslider.h \
    ./widgets/TF1DEditor.h \
    ./widgets/TF1DMappingCanvas.h \
    ./widgets/TF1DTextureCanvas.h \
    ./abstract/abstractsliceeditorplugin.h \
    ./widgets/histogramwidget.h \
    ./widgets/sliceeditorwidget.h \
    ./widgets/mainwindow.h \
    ./widgets/markcategorydialog.h \
    ./widgets/markinfowidget.h \
    ./model/markmodel.h \
    ./widgets/fileinfowidget.h \
    ./widgets/pixelwidget.h \
    ./widgets/profilewidget.h \
    ./widgets/slicewidget.h \
    ./widgets/titledsliderwithspinbox.h \
    ./widgets/zoomwidget.h \
    ./algorithm/CImg.h \
    ./widgets/TF1DMappingKey.h \
    ./abstract/abstractslicedatamodel.h \
    ./3drender/geometry/camera.h \
    ./model/categoryitem.h \
    ./globals.h \
    ./model/markitem.h \
    ./widgets/marktreeviewwidget.h \
    ./model/mrc.h \
    ./model/mrcdatamodel.h \
    ./model/sliceitem.h \
    ./model/treeitem.h \
    ./widgets/renderwidget.h \
    ./widgets/renderoptionwidget.h \
    ./widgets/slicetoolwidget.h \
    ./3drender/shader/shaderdatainterface.h \
    ./3drender/shader/shaderprogram.h \
    ./algorithm/gradientcalculator.h \
    ./3drender/shader/raycastingshader.h \
    ./3drender/geometry/mesh.h \
    ./3drender/geometry/geometry.h \
    ./3drender/geometry/volume.h \
    3drender/geometry/slicevolume.h \
    3drender/geometry/renderobject.h \
    algorithm/triangulate.h \
    algorithm/subdivion.h \
    widgets/slicecontrolwidget.h \
    widgets/slicemainwindow.h \
    widgets/volumemainwindow.h
SOURCES +=./widgets/doubleslider.cpp \
    ./widgets/TF1DEditor.cpp \
    ./widgets/TF1DMappingCanvas.cpp \
    ./widgets/TF1DTextureCanvas.cpp \
    ./abstract/abstractsliceeditorplugin.cpp \
    ./abstract/abstractslicedatamodel.cpp \
    ./model/categoryitem.cpp \
    ./widgets/histogramwidget.cpp \
    ./widgets/sliceeditorwidget.cpp \
    ./main.cpp \
    ./widgets/mainwindow.cpp \
    ./widgets/markcategray.cpp \
    ./widgets/markinfowidget.cpp \
    ./model/markitem.cpp \
    ./model/markmodel.cpp \
    ./widgets/marktreeviewwidget.cpp \
    ./model/mrc.cpp \
    ./model/mrcdatamodel.cpp \
    ./widgets/fileinfowidget.cpp \
    ./widgets/pixelwidget.cpp \
    ./widgets/profilewidget.cpp \
    ./model/sliceitem.cpp \
    ./widgets/slicewidget.cpp \
    ./widgets/titledsliderwithspinbox.cpp \
    ./model/treeitem.cpp \
    ./widgets/renderwidget.cpp \
    ./widgets/zoomwidget.cpp \
    ./widgets/renderoptionwidget.cpp \
    ./widgets/slicetoolwidget.cpp \
    ./algorithm/gradientcalculator.cpp \
    ./3drender/shader/shaderprogram.cpp \
    ./3drender/shader/raycastingshader.cpp \
    ./3drender/geometry/mesh.cpp \
    ./3drender/geometry/volume.cpp \
    3drender/geometry/slicevolume.cpp \
    3drender/geometry/renderobject.cpp \
    algorithm/triangulate.cpp \
    algorithm/subdivion.cpp \
    widgets/slicecontrolwidget.cpp \
    widgets/slicemainwindow.cpp \
    widgets/volumemainwindow.cpp
RESOURCES += resources.qrc
