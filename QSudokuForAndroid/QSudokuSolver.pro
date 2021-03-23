#-------------------------------------------------
#
# Project created by QtCreator 2018-10-02T21:20:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSudoku
TEMPLATE = app

ANDROID_ABIS = arm64-v8a

CONFIG(debug, debug|release){
    DEFINES += DEBUG_LOGOUT_ON
    message("Debug Build")
}

CONFIG(release, debug|release){
    message("Release Build")
}

#QMAKE_CFLAGS -= -MD
#QMAKE_CXXFLAGS -= -MD
#QMAKE_CFLAGS += -MT
#QMAKE_CXXFLAGS += -MT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES     += main.cpp\
    qsudokusolver.cpp \
    qsudoubox.cpp \
    solver.cpp \
    sudokupuzzles.cpp

HEADERS     += qsudokusolver.h \
    qsudoubox.h \
    solver.h \
    sudokupuzzles.h

FORMS       += qsudokusolver.ui

RESOURCES   += image.qrc

RC_FILE     += QSudokuSolver.rc

DISTFILES   += QSudokuSolver.rc \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
