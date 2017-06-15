QT += qml quick
CONFIG += c++14

SOURCES += main.cpp \
    crypto.cpp \
    crypto_man.cpp \
    session.cpp \
    session_server.cpp \
    qt_srv_interface.cpp \
    qtwindowinterface.cpp

RESOURCES += qml.qrc \
    lang.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += crypto.h \
    crypto_man.h \
    session.h \
    stdafx.h \
    main.h \
    threads.h

win32 {
    QMAKE_CXXFLAGS += -D_WIN32_WINNT=0x0501

    INCLUDEPATH += C:/cpplibs/include
    INCLUDEPATH += C:/boost

    CONFIG(release, debug|release) {
        LIBS += -LC:/cpplibs/lib64 -LC:/boost/lib64
    }

    CONFIG(debug, debug|release) {
        LIBS += -LC:/cpplibs/lib64d -LC:/boost/lib64
    }
}

android {
    QMAKE_CXXFLAGS += -DANDROID -DASIO_STANDALONE -fexceptions -frtti

    LIBS += -LD:/androidlibs/$$ANDROID_TARGET_ARCH
    LIBS += -lcryptopp
    INCLUDEPATH += C:/boost
    INCLUDEPATH += D:/androidlibs/include
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
