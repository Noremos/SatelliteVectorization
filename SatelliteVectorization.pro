QT += quick quickcontrols2

CONFIG += c++20

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += include
DEPENDPATH +=  include

#win32:CONFIG(release, debug|release): LIBS += -LD:/Programs/QT/ArctivViewer/ArcticViewer/ -lopencv_world412
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Programs/QT/ArctivViewer/ArcticViewer/ -lopencv_world412d
#else:unix: LIBS += -LD:/Programs/QT/ArctivViewer/ArcticViewer/ -lopencv_world412


SOURCES += \
        imageitem.cpp \
        source/barclasses.cpp \
        source/barcodeCreator.cpp \
        source/component.cpp \
        source/hole.cpp \
        backend.cpp \
        geodataprocessor.cpp \
        main.cpp

RESOURCES += qml.qrc
QMAKE_LFLAGS += -static-libgcc
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    imageitem.h \
    include/barImg.h \
    include/barclasses.h \
    include/barcodeCreator.h \
    include/barline.h \
    include/barscalar.h \
    include/barstrucs.h \
    include/component.h \
    include/hole.h \
    include/include_cv.h \
    include/include_py.h \
    include/presets.h \
    MatrImg.h \
    backend.h \
    enums.h \
    geodataprocessor.h


#win32:CONFIG(release, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411d
#else:unix: LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411

DISTFILES += \
    as.astylerc
