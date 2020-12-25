QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dataanalysisscene.cpp \
    destinationtableview.cpp \
    feebarchart.cpp \
    feepiechart.cpp \
    gridselectionview.cpp \
    main.cpp \
    mainwindow.cpp \
    mapview.cpp \
    predictduringview.cpp \
    querytableview.cpp \
    revenuebarchart.cpp \
    revenuepiechart.cpp \
    traveltimebarchart.cpp \
    traveltimepiechart.cpp

HEADERS += \
    dataanalysisscene.h \
    destinationtableview.h \
    feebarchart.h \
    feepiechart.h \
    gridselectionview.h \
    mainwindow.h \
    mapview.h \
    predictduringview.h \
    querytableview.h \
    revenuebarchart.h \
    revenuepiechart.h \
    traveltimebarchart.h \
    traveltimepiechart.h

FORMS += \
    dataanalysisscene.ui \
    gridselectionview.ui \
    mainwindow.ui \
    mapview.ui \
    predictduringview.ui \
    querytableview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
