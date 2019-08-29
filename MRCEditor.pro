TEMPLATE = app
TARGET = MRCEditor
QT += core opengl gui widgets

#CONFIG += console
CONFIG += c++14

INCLUDEPATH += src
 # source file name prefix
DEPENDPATH += src

# header file name prefix
MY_PREFIX_DIR = src

unix
{
    QMAKE_CXXFLAGS+= -fopenmp
    QMAKE_LFLAGS+= -fopenmp
}
#win32
#{
#QMAKE_CXXFLAGS+= -openmp
##QMAKE_LFLAGS+= -openmp
#}


HEADERS +=$${MY_PREFIX_DIR}/widgets/doubleslider.h\
    $${MY_PREFIX_DIR}/widgets/tf1deditor.h\
    $${MY_PREFIX_DIR}/widgets/tf1dmappingcanvas.h\
    $${MY_PREFIX_DIR}/widgets/tf1dtexturecanvas.h\
    $${MY_PREFIX_DIR}/abstract/abstractsliceeditorplugin.h\
    $${MY_PREFIX_DIR}/widgets/histogramwidget.h\
    $${MY_PREFIX_DIR}/widgets/sliceeditorwidget.h\
    $${MY_PREFIX_DIR}/widgets/mainwindow.h\
    $${MY_PREFIX_DIR}/widgets/markcategorydialog.h\
    $${MY_PREFIX_DIR}/widgets/markinfowidget.h\
    $${MY_PREFIX_DIR}/model/markmodel.h\
    $${MY_PREFIX_DIR}/widgets/fileinfowidget.h\
    $${MY_PREFIX_DIR}/widgets/pixelwidget.h\
    $${MY_PREFIX_DIR}/widgets/profilewidget.h\
    $${MY_PREFIX_DIR}/widgets/slicewidget.h \
    $${MY_PREFIX_DIR}/widgets/titledsliderwithspinbox.h \
    $${MY_PREFIX_DIR}/widgets/zoomwidget.h \
    $${MY_PREFIX_DIR}/algorithm/CImg.h \
    $${MY_PREFIX_DIR}/widgets/tf1dmappingkey.h \
    $${MY_PREFIX_DIR}/abstract/abstractslicedatamodel.h\
    $${MY_PREFIX_DIR}/3drender/geometry/camera.h \
    $${MY_PREFIX_DIR}/model/categorytreeitem.h \
    $${MY_PREFIX_DIR}/globals.h \
    $${MY_PREFIX_DIR}/model/markitem.h \
    $${MY_PREFIX_DIR}/widgets/marktreeviewwidget.h \
    $${MY_PREFIX_DIR}/model/mrc.h \
    $${MY_PREFIX_DIR}/model/mrcdatamodel.h \
    $${MY_PREFIX_DIR}/model/sliceitem.h \
    $${MY_PREFIX_DIR}/model/treeitem.h \
    $${MY_PREFIX_DIR}/widgets/renderwidget.h \
    $${MY_PREFIX_DIR}/widgets/renderoptionwidget.h \
    $${MY_PREFIX_DIR}/widgets/categorycontrolwidget.h \
    $${MY_PREFIX_DIR}/3drender/shader/shaderdatainterface.h \
    $${MY_PREFIX_DIR}/3drender/shader/shaderprogram.h \
    $${MY_PREFIX_DIR}/algorithm/gradientcalculator.h \
    $${MY_PREFIX_DIR}/3drender/shader/raycastingshader.h \
    $${MY_PREFIX_DIR}/3drender/geometry/mesh.h \
    $${MY_PREFIX_DIR}/3drender/geometry/volume.h \
    $${MY_PREFIX_DIR}/3drender/geometry/slicevolume.h \
    $${MY_PREFIX_DIR}/3drender/geometry/renderobject.h \
    $${MY_PREFIX_DIR}/algorithm/triangulate.h \
    $${MY_PREFIX_DIR}/algorithm/subdivion.h \
    $${MY_PREFIX_DIR}/widgets/slicecontrolwidget.h \
    $${MY_PREFIX_DIR}/widgets/slicemainwindow.h \
    $${MY_PREFIX_DIR}/widgets/volumemainwindow.h \
    $${MY_PREFIX_DIR}/model/instanceitem.h \
    $${MY_PREFIX_DIR}/model/roottreeitem.h \
    $${MY_PREFIX_DIR}/model/marktreeitem.h \
    $${MY_PREFIX_DIR}/model/iteminfomodel.h \
    $${MY_PREFIX_DIR}/model/strokeitem.h \
    $${MY_PREFIX_DIR}/algorithm/bimap.h \
    $${MY_PREFIX_DIR}/widgets/colorlisteditor.h \
    $${MY_PREFIX_DIR}/abstract/widgetplugininterface.h \
    $${MY_PREFIX_DIR}/mathematics/basetype.h \
    $${MY_PREFIX_DIR}/mathematics/numeric.h \
    $${MY_PREFIX_DIR}/mathematics/arithmetic.h \
    $${MY_PREFIX_DIR}/mathematics/geometry.h \
    $${MY_PREFIX_DIR}/mathematics/transformation.h \
    $${MY_PREFIX_DIR}/model/commands.h \
    

SOURCES +=$${MY_PREFIX_DIR}/widgets/doubleslider.cpp \
    $${MY_PREFIX_DIR}/widgets/tf1deditor.cpp \
    $${MY_PREFIX_DIR}/widgets/tf1dmappingcanvas.cpp \
    $${MY_PREFIX_DIR}/widgets/tf1dtexturecanvas.cpp \
    $${MY_PREFIX_DIR}/abstract/abstractsliceeditorplugin.cpp \
    $${MY_PREFIX_DIR}/abstract/abstractslicedatamodel.cpp \
    $${MY_PREFIX_DIR}/model/categorytreeitem.cpp \
    $${MY_PREFIX_DIR}/widgets/histogramwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/sliceeditorwidget.cpp \
    $${MY_PREFIX_DIR}/main.cpp \
    $${MY_PREFIX_DIR}/widgets/mainwindow.cpp \
    $${MY_PREFIX_DIR}/widgets/markcategray.cpp \
    $${MY_PREFIX_DIR}/widgets/markinfowidget.cpp \
    $${MY_PREFIX_DIR}/model/markitem.cpp \
    $${MY_PREFIX_DIR}/model/markmodel.cpp \
    $${MY_PREFIX_DIR}/widgets/marktreeviewwidget.cpp \
    $${MY_PREFIX_DIR}/model/mrc.cpp \
    $${MY_PREFIX_DIR}/model/mrcdatamodel.cpp \
    $${MY_PREFIX_DIR}/widgets/fileinfowidget.cpp \
    $${MY_PREFIX_DIR}/widgets/pixelwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/profilewidget.cpp \
    $${MY_PREFIX_DIR}/model/sliceitem.cpp \
    $${MY_PREFIX_DIR}/widgets/slicewidget.cpp \
    $${MY_PREFIX_DIR}/widgets/titledsliderwithspinbox.cpp \
    $${MY_PREFIX_DIR}/model/treeitem.cpp \
    $${MY_PREFIX_DIR}/widgets/renderwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/zoomwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/renderoptionwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/categorycontrolwidget.cpp \
    $${MY_PREFIX_DIR}/algorithm/gradientcalculator.cpp \
    $${MY_PREFIX_DIR}/3drender/shader/shaderprogram.cpp \
    $${MY_PREFIX_DIR}/3drender/shader/raycastingshader.cpp \
    $${MY_PREFIX_DIR}/3drender/geometry/mesh.cpp \
    $${MY_PREFIX_DIR}/3drender/geometry/volume.cpp \
    $${MY_PREFIX_DIR}/3drender/geometry/slicevolume.cpp \
    $${MY_PREFIX_DIR}/3drender/geometry/renderobject.cpp \
    $${MY_PREFIX_DIR}/algorithm/triangulate.cpp \
    $${MY_PREFIX_DIR}/algorithm/subdivion.cpp \
    $${MY_PREFIX_DIR}/widgets/slicecontrolwidget.cpp \
    $${MY_PREFIX_DIR}/widgets/slicemainwindow.cpp \
    $${MY_PREFIX_DIR}/widgets/volumemainwindow.cpp \
    $${MY_PREFIX_DIR}/model/instanceitem.cpp \
    $${MY_PREFIX_DIR}/model/roottreeitem.cpp \
    $${MY_PREFIX_DIR}/model/marktreeitem.cpp \
    $${MY_PREFIX_DIR}/model/iteminfomodel.cpp \
    $${MY_PREFIX_DIR}/model/strokeitem.cpp \
    $${MY_PREFIX_DIR}/algorithm/bimap.cpp \
    $${MY_PREFIX_DIR}/widgets/colorlisteditor.cpp \
    $${MY_PREFIX_DIR}/abstract/widgetplugininterface.cpp \
    $${MY_PREFIX_DIR}/mathematics/transformation.cpp \
    $${MY_PREFIX_DIR}/mathematics/arithmetic.cpp \
    $${MY_PREFIX_DIR}/mathematics/geometry.cpp \
    $${MY_PREFIX_DIR}/model/commands.cpp \

RESOURCES += resources.qrc
