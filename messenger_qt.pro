QT += qml quick

SOURCES += main.cpp \
    crypto.cpp \
    crypto_man.cpp \
    session.cpp \
    session_server.cpp \
    qt_srv_interface.cpp \
    qtwindowinterface.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    crypto.h \
    crypto_man.h \
    session.h \
    stdafx.h \
    main.h \
    threads.h

win32 {
    QMAKE_CXXFLAGS += -D_WIN32_WINNT=0x0501
    LIBS += -LC:/cpplibs/lib64 -LC:/boost/lib64
    INCLUDEPATH += C:/cpplibs/include
    INCLUDEPATH += C:/boost
}

android {
    QMAKE_CXXFLAGS += -std=c++14 -DNO_STD_TOSTRING

    equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
        LIBS += -LD:/androidlibs/arm -LD:/androidlibs/arm/boost
    }
    equals(ANDROID_TARGET_ARCH, x86)  {
        LIBS += -LD:/androidlibs/x86 -LD:/androidlibs/x86/boost
    }
    LIBS += -lcryptopp -lboost_system
    INCLUDEPATH += D:/androidlibs/include
    INCLUDEPATH += C:/boost
}
