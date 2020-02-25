QT       += core gui xml sql script

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
    aux/utility.cpp \
    dao/libdao.cpp \
    dao/mdldao.cpp \
    dao/udldao.cpp \
    dao/welldao.cpp \
    dto/mdlfield.cpp \
    dto/mdltable.cpp \
    dto/mdltablechildren.cpp \
    dto/mdlunit.cpp \
    dto/record.cpp \
    main.cpp \
    model/qt/qexsortfilterproxymodel.cpp \
    model/qwmrotatableproxymodel.cpp \
    model/qwmsortfilterproxymodel.cpp \
    model/qwmtablemodel.cpp \
    qwmapplication.cpp \
    qwmmain.cpp \
    ui/qwmdataeditor.cpp \
    undo/qfieldeditcommand.cpp \
    widget/delegate/comboboxdelegate.cpp \
    widget/delegate/qwmdatedelegate.cpp \
    widget/delegate/qwmliblookupdelegate.cpp \
    widget/delegate/spinboxdelegate.cpp \
    widget/editor/qwmdatetimeeditor.cpp \
    widget/editor/qwmlibselector.cpp \
    widget/qdlgwellfieldsselector.cpp \
    widget/qwmdatatableview.cpp \

HEADERS += \
    aux/utility.h \
    common.h \
    dao/libdao.h \
    dao/mdldao.h \
    dao/udldao.h \
    dao/welldao.h \
    dto/dto.h \
    dto/mdlfield.h \
    dto/mdltable.h \
    dto/mdltablechildren.h \
    dto/mdlunit.h \
    dto/record.h \
    model/qt/qexsortfilterproxymodel.h \
    model/qwmrotatableproxymodel.h \
    model/qwmsortfilterproxymodel.h \
    model/qwmtablemodel.h \
    qwmapplication.h \
    qwmmain.h \
    ui/qwmdataeditor.h \
    undo/qfieldeditcommand.h \
    widget/delegate/comboboxdelegate.h \
    widget/delegate/qwmdatedelegate.h \
    widget/delegate/qwmliblookupdelegate.h \
    widget/delegate/spinboxdelegate.h \
    widget/editor/qwmdatetimeeditor.h \
    widget/editor/qwmlibselector.h \
    widget/qdlgwellfieldsselector.h \
    widget/qwmdatatableview.h \

FORMS += \
    qwmmain.ui \
    ui/qwmdataeditor.ui \
    widget/editor/qwmdatetimeeditor.ui \
    widget/editor/qwmlibselector.ui \
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

INCLUDEPATH += $$PWD/dao $$PWD/widget $$PWD/widget/delegate $$PWD/widget/editor $$PWD/ui $$PWD/undo $$PWD/aux $$PWD/model $$PWD/dto  $$PWD/model/qt

INCLUDEPATH += /Users/Allein/Qt/5.14.0/lib/QtCore.framework/Versions/5/Headers/5.14.0
INCLUDEPATH += /Users/Allein/Qt/5.14.0/lib/QtCore.framework/Versions/5/Headers/5.14.0/QtCore
DEFINES += QT_MESSAGELOGCONTEXT
