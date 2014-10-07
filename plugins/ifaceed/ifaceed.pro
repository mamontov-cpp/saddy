######################################################################
# Automatically generated by qmake (2.01a) ?? ????. 14 17:12:28 2013
######################################################################

QT += core gui xml testlib opengl
CONFIG += console opengl
TEMPLATE = app
TARGET = 
DEPENDPATH += . \
              ifaceed \
              ifaceed/core \
              ifaceed/editorcore \
              ifaceed/gui \
              ifaceed/history \
              ifaceed/objects \
              ifaceed/unittests \
              ifaceed/core/states \
              ifaceed/gui/spritewidget
INCLUDEPATH += . \
               ifaceed \
               ifaceed/gui/spritewidget \
               ifaceed/core \
               ifaceed/editorcore \
               ifaceed/gui \
               ifaceed/history \
               ifaceed/unittests \
               ifaceed/objects \
               ifaceed/core/states \
	       ../../include/ \
	       ../../include/config/ \
	       ../../include/log/ \
	       ../../include/marshal/ \
	       ../../include/os/ \
	       ../../include/primitives/ \
	       ../../include/templates/ \

# Input
HEADERS += ifaceed/mainpanel.h \
    ifaceed/core/typeconverters/load.h \
    ifaceed/core/typeconverters/qcolortosadacolor.h \
    ifaceed/core/typeconverters/qcolortosadcolor.h \
    ifaceed/core/typeconverters/qlistqlistqcolortosadvectorsadvectoracolor.h \
    ifaceed/core/typeconverters/qstringtosadstring.h \
    ifaceed/core/typeconverters/sadacolortoqcolor.h \
    ifaceed/core/typeconverters/sadcolortoqcolor.h \
    ifaceed/core/typeconverters/sadstringtoqstring.h \
    ifaceed/core/typeconverters/sadvectorsadvectoracolortoqlistqlistqcolor.h \
    ifaceed/core/typeconverters/save.h \
    ifaceed/core/editor.h \
    ifaceed/core/editorbehaviour.h \
    ifaceed/core/qttarget.h \
    ifaceed/core/quitreason.h \
    ifaceed/core/saddythread.h \
    ifaceed/core/shared.h \
    ifaceed/core/state.h \
    ifaceed/acolordialog.h \
    ifaceed/macro.h \
    ifaceed/qwwlongspinbox.h \
    ifaceed/qwwulongspinbox.h \
    ifaceed/wwglobal.h \
    ifaceed/wwglobal_p.h \
    ifaceed/history/command.h \
    ifaceed/history/history.h \
    ifaceed/history/database/newproperty.h \
    ifaceed/history/database/propertychanged.h \
    ifaceed/history/database/removeproperty.h \
    ifaceed/gui/anglewidget/anglewidget.h \
    ifaceed/gui/colorpicker/colorpicker.h \
    ifaceed/gui/colorview/colorview.h \
    ifaceed/gui/fontsizewidget/fontsizewidget.h \
    ifaceed/gui/rectwidget/rectwidget.h \
    ifaceed/gui/resourcetreewidget/cell.h \
    ifaceed/gui/resourcetreewidget/celldelegate.h \
    ifaceed/gui/resourcetreewidget/defaultimage.h \
    ifaceed/gui/resourcetreewidget/resourcecache.h \
    ifaceed/gui/resourcetreewidget/resourcetreewidget.h \
    ifaceed/gui/table/delegate.h \
    ifaceed/gui/table/delegatefactory.h \
    ifaceed/gui/table/doubledelegate.h \
    ifaceed/gui/table/floatdelegate.h \
    ifaceed/gui/table/intdelegate.h \
    ifaceed/gui/table/longdelegate.h \
    ifaceed/gui/table/longlongdelegate.h \
    ifaceed/gui/table/sadacolordelegate.h \
    ifaceed/gui/table/sadcolordelegate.h \
    ifaceed/gui/table/sadstringdelegate.h \
    ifaceed/gui/table/shortdelegate.h \
    ifaceed/gui/table/signedchardelegate.h \
    ifaceed/gui/table/unsignedchardelegate.h \
    ifaceed/gui/table/unsignedintdelegate.h \
    ifaceed/gui/table/unsignedlongdelegate.h \
    ifaceed/gui/table/unsignedlonglongdelegate.h \
    ifaceed/gui/table/unsignedshortdelegate.h \
    ifaceed/gui/tuplewidget/doubletuplewidget.h \
    ifaceed/gui/tuplewidget/int64tuplewidget.h \
    ifaceed/gui/table/sadpoint2ddelegate.h \
    ifaceed/gui/table/sadsize2ddelegate.h \
    ifaceed/gui/table/sadpoint2idelegate.h \
    ifaceed/gui/table/sadsize2idelegate.h \
    ifaceed/gui/eventfilter.h \
    ifaceed/history/scenes/scenesadd.h \
    ifaceed/history/scenes/sceneschangename.h \
    ifaceed/history/scenes/sceneslayerswap.h \
    ifaceed/history/scenes/scenesremove.h \
    ifaceed/core/synchronization.h \
    ifaceed/history/scenenodes/scenenodesnew.h \
    ifaceed/gui/labelactions.h \
    ifaceed/gui/scenenodeactions.h \
    ifaceed/core/borders/activeborder.h \
    ifaceed/core/borders/border.h \
    ifaceed/core/borders/hotspots.h \
    ifaceed/core/borders/selectionborder.h \
    ifaceed/history/label/changefontname.h \
    ifaceed/closuremethodcall.h \
    ifaceed/blockedclosuremethodcall.h \
    ifaceed/history/label/changefontsize.h \
    ifaceed/history/label/changetext.h \
    ifaceed/history/label/changelinespacing.h \
    ifaceed/history/scenenodes/scenenodeschangename.h \
    ifaceed/core/typeconverters/sadrect2dtoqrectf.h \
    ifaceed/core/typeconverters/qrectftosadrect2d.h \
    ifaceed/history/scenenodes/scenenodeschangearea.h \
    ifaceed/history/database/changeproperty.h \
    ifaceed/history/scenenodes/scenenodeschangeangle.h \
    ifaceed/history/scenenodes/scenenodeschangecolor.h \
    ifaceed/history/scenenodes/scenenodeschangeproperty.h \
    ifaceed/history/scenenodes/scenenodeschangepropertywhichlinkedtocheckbox.h \
    ifaceed/history/scenenodes/scenenodeschangevisibility.h \
    ifaceed/gui/rotationprocess.h \
    ifaceed/gui/sprite2dactions.h \
    ifaceed/history/sprite2d/changeflipx.h \
    ifaceed/history/sprite2d/changeflipy.h \
    ifaceed/history/sprite2d/changeoptions.h \
    ifaceed/history/sprite2d/makebackground.h \
    ifaceed/history/scenenodes/scenenodesremove.h \
    ifaceed/history/scenenodes/scenenodeslayerswap.h \
    ifaceed/history/scenes/scenesclear.h \
    ifaceed/history/customobject/customobjectchangeproperty.h \
    ifaceed/gui/customobjectactions.h \
    ifaceed/history/customobject/customobjectchangeschema.h \
    ifaceed/gui/updateelement.h \
    ifaceed/core/borders/deletehotspot.h \
    ifaceed/core/borders/resizehotspot.h \
    ifaceed/core/selection.h \
    ifaceed/reloadfilelist.h
FORMS += ifaceed/mainpanel.ui \
    ifaceed/acolordialog.ui \
    ifaceed/reloadfilelist.ui
SOURCES += ifaceed/main.cpp \
           ifaceed/mainpanel.cpp \
    ifaceed/core/typeconverters/qcolortosadacolor.cpp \
    ifaceed/core/typeconverters/qcolortosadcolor.cpp \
    ifaceed/core/typeconverters/qlistqlistqcolortosadvectorsadvectoracolor.cpp \
    ifaceed/core/typeconverters/qstringtosadstring.cpp \
    ifaceed/core/typeconverters/sadacolortoqcolor.cpp \
    ifaceed/core/typeconverters/sadcolortoqcolor.cpp \
    ifaceed/core/typeconverters/sadstringtoqstring.cpp \
    ifaceed/core/typeconverters/sadvectorsadvectoracolortoqlistqlistqcolor.cpp \
    ifaceed/core/editor.cpp \
    ifaceed/core/qttarget.cpp \
    ifaceed/core/saddythread.cpp \
    ifaceed/acolordialog.cpp \
    ifaceed/qwwlongspinbox.cpp \
    ifaceed/qwwulongspinbox.cpp \
    ifaceed/wwglobal_p.cpp \
    ifaceed/history/command.cpp \
    ifaceed/history/history.cpp \
    ifaceed/history/database/newproperty.cpp \
    ifaceed/history/database/removeproperty.cpp \
    ifaceed/gui/anglewidget/anglewidget.cpp \
    ifaceed/gui/colorpicker/colorpicker.cpp \
    ifaceed/gui/colorview/colorview.cpp \
    ifaceed/gui/fontsizewidget/fontsizewidget.cpp \
    ifaceed/gui/rectwidget/rectwidget.cpp \
    ifaceed/gui/resourcetreewidget/cell.cpp \
    ifaceed/gui/resourcetreewidget/celldelegate.cpp \
    ifaceed/gui/resourcetreewidget/defaultimage.cpp \
    ifaceed/gui/resourcetreewidget/resourcecache.cpp \
    ifaceed/gui/resourcetreewidget/resourcetreewidget.cpp \
    ifaceed/gui/table/delegate.cpp \
    ifaceed/gui/table/delegatefactory.cpp \
    ifaceed/gui/table/doubledelegate.cpp \
    ifaceed/gui/table/floatdelegate.cpp \
    ifaceed/gui/table/intdelegate.cpp \
    ifaceed/gui/table/longdelegate.cpp \
    ifaceed/gui/table/longlongdelegate.cpp \
    ifaceed/gui/table/sadacolordelegate.cpp \
    ifaceed/gui/table/sadcolordelegate.cpp \
    ifaceed/gui/table/sadstringdelegate.cpp \
    ifaceed/gui/table/shortdelegate.cpp \
    ifaceed/gui/table/signedchardelegate.cpp \
    ifaceed/gui/table/unsignedchardelegate.cpp \
    ifaceed/gui/table/unsignedintdelegate.cpp \
    ifaceed/gui/table/unsignedlongdelegate.cpp \
    ifaceed/gui/table/unsignedlonglongdelegate.cpp \
    ifaceed/gui/table/unsignedshortdelegate.cpp \
    ifaceed/gui/tuplewidget/doubletuplewidget.cpp \
    ifaceed/gui/tuplewidget/int64tuplewidget.cpp \
    ifaceed/gui/table/sadpoint2ddelegate.cpp \
    ifaceed/gui/table/sadsize2ddelegate.cpp \
    ifaceed/gui/table/sadpoint2idelegate.cpp \
    ifaceed/gui/table/sadsize2idelegate.cpp \
    ifaceed/gui/eventfilter.cpp \
    ifaceed/history/scenes/scenelayerswap.cpp \
    ifaceed/history/scenes/scenesadd.cpp \
    ifaceed/history/scenes/sceneschangename.cpp \
    ifaceed/history/scenes/scenesremove.cpp \
    ifaceed/core/synchronization.cpp \
    ifaceed/history/scenenodes/scenenodesnew.cpp \
    ifaceed/gui/labelactions.cpp \
    ifaceed/gui/scenenodeactions.cpp \
    ifaceed/core/borders/activeborder.cpp \
    ifaceed/core/borders/border.cpp \
    ifaceed/core/borders/selectionborder.cpp \
    ifaceed/core/shared.cpp \
    ifaceed/history/label/changefontname.cpp \
    ifaceed/history/label/changefontsize.cpp \
    ifaceed/history/label/changetext.cpp \
    ifaceed/history/label/changelinespacing.cpp \
    ifaceed/history/scenenodes/scenesnodeschangename.cpp \
    ifaceed/core/typeconverters/sadrect2dtoqrectf.cpp \
    ifaceed/core/typeconverters/qrectftosadrect2d.cpp \
    ifaceed/history/scenenodes/scenenodeschangearea.cpp \
    ifaceed/history/scenenodes/scenenodeschangeangle.cpp \
    ifaceed/history/scenenodes/scenenodeschangecolor.cpp \
    ifaceed/history/scenenodes/scenenodeschangepropertywhichlinkedtocheckbox.cpp \
    ifaceed/history/scenenodes/scenenodeschangevisibility.cpp \
    ifaceed/gui/rotationprocess.cpp \
    ifaceed/gui/sprite2dactions.cpp \
    ifaceed/history/sprite2d/changeflipx.cpp \
    ifaceed/history/sprite2d/changeflipy.cpp \
    ifaceed/history/sprite2d/changeoptions.cpp \
    ifaceed/history/sprite2d/makebackground.cpp \
    ifaceed/history/scenenodes/scenenodesremove.cpp \
    ifaceed/history/scenenodes/scenenodeslayerswap.cpp \
    ifaceed/history/scenes/scenesclear.cpp \
    ifaceed/gui/customobjectactions.cpp \
    ifaceed/history/customobject/customobjectchangeschema.cpp \
    ifaceed/core/borders/deletehotspot.cpp \
    ifaceed/core/borders/resizehotspot.cpp \
    ifaceed/core/selection.cpp \
    ifaceed/reloadfilelist.cpp


DESTDIR = ../../bin/ifaceed/

unix {
	DEFINES += "UNIX=1"
	DEFINES += "LINUX=1"
	DEFINES += "GCC=1"
}

win32 {
	DEFINES += "WIN32=1"
	DEFINES +=  "MINGW=1"
}

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/ -lsaddy-debug -lsaddy-ft-debug
    TARGET = ifaceeditor-debug
}

CONFIG(release, debug|release) {
    LIBS += -L../../lib/ -lsaddy-release -lsaddy-ft-release
    TARGET = ifaceeditor-release
}

unix {
        LIBS += -lGL
        LIBS += -lGLU
}

win32 {
        QMAKE_LIBDIR += $(FREETYPE_LIB)
        LIBS += -lglu32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool  -lshell32 -lcomdlg32 -ladvapi32 -lfreetype
}

QMAKE_CXXFLAGS += -Wno-reorder -Wno-unused -Wno-sign-compare
