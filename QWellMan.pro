QT       += core gui xml sql
QT       += script
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ui/qwmprogressdialog.cpp \
    ui/qwmreportor.cpp \
    util/qwmcalculator.cpp \
    util/qwmuserdata.cpp \
    util/utility.cpp \
    dao/edldao.cpp \
    dao/libdao.cpp \
    dao/mdldao.cpp \
    dao/udldao.cpp \
    dao/welldao.cpp \
    dto/mdlfield.cpp \
    dto/mdlfieldlookup.cpp \
    dto/mdltable.cpp \
    dto/mdltablechildren.cpp \
    dto/mdlunit.cpp \
    dto/record.cpp \
    dto/udllibgroup.cpp \
    dto/udllibtab.cpp \
    dto/udllibtabfield.cpp \
    dto/udltableproperty.cpp \
    main.cpp \
    model/qwmlibquerymodel.cpp \
    model/qwmrotatableproxymodel.cpp \
    model/qwmsortfilterproxymodel.cpp \
    model/qwmtablemodel.cpp \
    qwmapplication.cpp \
    qwmmain.cpp \
    ui/qwmabout.cpp \
    ui/qwmdataeditor.cpp \
    undo/qwmfieldeditcommand.cpp \
    undo/qwmrecordeditcommand.cpp \
    widget/delegate/qwmabstractdelegate.cpp \
    widget/delegate/qwmreflookupdelegate.cpp \
    widget/editor/qwmabstracteditor.cpp \
    widget/editor/qwmdatetimeeditor.cpp \
    widget/editor/qwmiconselector.cpp \
    widget/editor/qwmlibselector.cpp \
    widget/editor/qwmlibsingleselector.cpp \
    widget/editor/qwmlibtabselector.cpp \
    widget/editor/qwmrecordselector.cpp \
    widget/editor/qwmrecordsinglestepselector.cpp \
    widget/editor/qwmrecordtwostepselector.cpp \
    widget/editor/qwmtableselector.cpp \
    widget/graphics/qwmcurveswidget.cpp \
    widget/graphics/qwmdataserialswidget.cpp \
    widget/graphics/qwmgeographicsscene.cpp \
    widget/graphics/qwmgeosimpleformation.cpp \
    widget/graphics/qwmgeotrackcontent.cpp \
    widget/graphics/qwmgeotracktitle.cpp \
    widget/graphics/qwmgeotrackwidget.cpp \
    widget/graphics/qwmgeowellboresection.cpp \
    widget/graphics/qwmgeowellboreverticalsection.cpp \
    widget/qdlgwellfieldsselector.cpp \
    widget/qwmdatatableview.cpp \
    widget/qwmheaderview.cpp

HEADERS += \
    ui/qwmprogressdialog.h \
    ui/qwmreportor.h \
    util/qwmcalculator.h \
    util/qwmuserdata.h \
    util/utility.h \
    common.h \
    dao/edldao.h \
    dao/libdao.h \
    dao/mdldao.h \
    dao/udldao.h \
    dao/welldao.h \
    dto/dto.h \
    dto/mdlfield.h \
    dto/mdlfieldlookup.h \
    dto/mdltable.h \
    dto/mdltablechildren.h \
    dto/mdlunit.h \
    dto/record.h \
    dto/udllibgroup.h \
    dto/udllibtab.h \
    dto/udllibtabfield.h \
    dto/udltableproperty.h \
    model/qwmlibquerymodel.h \
    model/qwmrotatableproxymodel.h \
    model/qwmsortfilterproxymodel.h \
    model/qwmtablemodel.h \
    qwmapplication.h \
    qwmmain.h \
    ui/qwmabout.h \
    ui/qwmdataeditor.h \
    undo/qwmfieldeditcommand.h \
    undo/qwmrecordeditcommand.h \
    widget/delegate/qwmabstractdelegate.h \
    widget/delegate/qwmreflookupdelegate.h \
    widget/editor/qwmabstracteditor.h \
    widget/editor/qwmdatetimeeditor.h \
    widget/editor/qwmiconselector.h \
    widget/editor/qwmlibselector.h \
    widget/editor/qwmlibsingleselector.h \
    widget/editor/qwmlibtabselector.h \
    widget/editor/qwmrecordselector.h \
    widget/editor/qwmrecordsinglestepselector.h \
    widget/editor/qwmrecordtwostepselector.h \
    widget/editor/qwmtableselector.h \
    widget/graphics/qwmcurveswidget.h \
    widget/graphics/qwmdataserialswidget.h \
    widget/graphics/qwmgeocurveinfo.h \
    widget/graphics/qwmgeoformationinfo.h \
    widget/graphics/qwmgeographicsscene.h \
    widget/graphics/qwmgeosimpleformation.h \
    widget/graphics/qwmgeotrackcontent.h \
    widget/graphics/qwmgeotracktitle.h \
    widget/graphics/qwmgeotrackwidget.h \
    widget/graphics/qwmgeowellboresection.h \
    widget/graphics/qwmgeowellboreverticalsection.h \
    widget/qdlgwellfieldsselector.h \
    widget/qwmdatatableview.h \
    widget/qwmheaderview.h

FORMS += \
    qwmmain.ui \
    ui/about.ui \
    ui/qwmdataeditor.ui \
    ui/qwmprogressdialog.ui \
    ui/qwmreportor.ui \
    widget/editor/qwmdatetimeeditor.ui \
    widget/editor/qwmiconselector.ui \
    widget/editor/qwmlibselector.ui \
    widget/editor/qwmlibsingleselector.ui \
    widget/editor/qwmlibtabselector.ui \
    widget/editor/qwmrecordselector.ui \
    widget/editor/qwmrecordsinglestepselector.ui \
    widget/editor/qwmrecordtowstepselector.ui \
    widget/editor/qwmtableselector.ui \
    widget/qdlgwellfieldsselector.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc

TRANSLATIONS +=  \
    $$PWD/resource/i18n/QWellMan_zh_CN.ts \

DISTFILES += \
    resource/i18n/QWellMan_zh_CN.ts

#RC_ICONS = logo128.png

INCLUDEPATH += $$PWD/dao $$PWD/widget $$PWD/widget/delegate $$PWD/widget/editor $$PWD/ui $$PWD/undo $$PWD/util $$PWD/model $$PWD/dto  $$PWD/model/qt
INCLUDEPATH += $$PWD/widget/graphics
#INCLUDEPATH += /Users/Allein/Qt/5.14.0/lib/QtCore.framework/Versions/5/Headers/5.14.0
#INCLUDEPATH += /Users/Allein/Qt/5.14.0/lib/QtCore.framework/Versions/5/Headers/5.14.0/QtCore
#INCLUDEPATH += /Users/Allein/Qt/5.14.0/lib/QtWidgets.framework/Versions/5/Headers/5.14.0/QtWidgets
#INCLUDEPATH += /Users/Allein/Qt/5.14.0//lib/QtWidgets.framework/Versions/5/Headers/5.14.0
#INCLUDEPATH += /Users/Allein/Qt/5.14.0//lib/QtGui.framework/Versions/5/Headers/5.14.0
#INCLUDEPATH += /Users/Allein/Qt/5.14.0//lib/QtGui.framework/Versions/5/Headers/5.14.0/QtGui
#INCLUDEPATH += %{Qt:QT_INSTALL_PREFIX}/lib/QtWidgets.framework/Versions/5/Headers/5.14.0/QtWidgets
DEFINES += QT_MESSAGELOGCONTEXT

CONFIG(debug, debug|release) {
DEFINES +=  DEBUG
} else {

}


