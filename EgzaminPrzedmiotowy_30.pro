QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    W_DANYCH\importer.cpp \
    W_DANYCH\importerBlokowy.cpp \
    main.cpp \
    W_PREZENTACJI\mainwindow.cpp \
    W_USLUG\program.cpp \
    W_DANYCH\pytanie.cpp \
    W_DANYCH\urn.cpp

HEADERS += \
    W_DANYCH\importer.h \
    W_DANYCH\importerBlokowy.h \
    W_PREZENTACJI\mainwindow.h \
    W_USLUG\program.h \
    W_DANYCH\pytanie.h \
    W_DANYCH\urn.h

FORMS += \
    W_PREZENTACJI\mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    PytaniaNEW/PK/BlokA \
    PytaniaNEW/PK/BlokB \
    PytaniaNEW/PPK/BlokA \
    PytaniaOLD/PK/A-01 \
    PytaniaOLD/PK/A-02 \
    PytaniaOLD/PK/A-03 \
    PytaniaOLD/PK/A-04 \
    PytaniaOLD/PK/A-05 \
    PytaniaOLD/PK/A-06 \
    PytaniaOLD/PK/A-07 \
    PytaniaOLD/PK/A-08 \
    PytaniaOLD/PK/A-09 \
    PytaniaOLD/PK/A-10 \
    PytaniaOLD/PK/A-11 \
    PytaniaOLD/PK/A-12 \
    PytaniaOLD/PK/A-13 \
    PytaniaOLD/PK/B-01 \
    PytaniaOLD/PK/B-02 \
    PytaniaOLD/PK/B-03 \
    PytaniaOLD/PK/B-04 \
    PytaniaOLD/PK/B-05 \
    PytaniaOLD/PK/B-06 \
    PytaniaOLD/PK/B-07 \
    PytaniaOLD/PK/B-08 \
    PytaniaOLD/PK/B-09 \
    PytaniaOLD/PK/B-10 \
    PytaniaOLD/PK/B-11 \
    PytaniaOLD/PK/B-12 \
    PytaniaOLD/PK/B-13 \
    PytaniaOLD/PPK/A-01 \
    PytaniaOLD/PPK/A-02 \
    PytaniaOLD/PPK/A-03 \
    PytaniaOLD/PPK/A-04 \
    PytaniaOLD/PPK/A-05 \
    PytaniaOLD/PPK/A-06 \
    PytaniaOLD/PPK/A-07 \
    PytaniaOLD/PPK/A-08
