TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_pkgconfig

PKGCONFIG += libola

LIBS += -lpigpiod_if2

SOURCES += main.cpp \
    OlaToRpiMulti.cpp \
    RpiDmxOutput.cpp

HEADERS += \
    OlaToRpiMulti.hpp \
    RpiDmxOutput.hpp

DISTFILES += \
    README.md \
    LICENSE
