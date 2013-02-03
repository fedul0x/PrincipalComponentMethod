HEADERS = mainwindow.h \
    mdichild.h \
    gexception.h \
    graph.h \
    gnode.h \
    gfileexception.h \
    alglib/stdafx.h \
    alglib/statistics.h \
    alglib/specialfunctions.h \
    alglib/solvers.h \
    alglib/integration.h \
    alglib/fasttransforms.h \
    alglib/diffequations.h \
    alglib/dataanalysis.h \
    alglib/optimization.h \
    alglib/linalg.h \
    alglib/interpolation.h \
    alglib/ap.h \
    alglib/alglibmisc.h \
    alglib/alglibinternal.h
SOURCES = main.cpp \
    mainwindow.cpp \
    mdichild.cpp \
    gexception.cpp \
    graph.cpp \
    gnode.cpp \
    gfileexception.cpp \
    alglib/statistics.cpp \
    alglib/specialfunctions.cpp \
    alglib/integration.cpp \
    alglib/fasttransforms.cpp \
    alglib/diffequations.cpp \
    alglib/solvers.cpp \
    alglib/optimization.cpp \
    alglib/linalg.cpp \
    alglib/interpolation.cpp \
    alglib/dataanalysis.cpp \
    alglib/ap.cpp \
    alglib/alglibmisc.cpp \
    alglib/alglibinternal.cpp
RESOURCES = mdi.qrc
TRANSLATIONS += mdi.ts

# install
target.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/mdi
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    mdi.pro \
    images
sources.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/mdi
INSTALLS += target \
    sources
symbian:include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
include(log4qt/log4qt.pri)
OTHER_FILES += ap.o \
    mdi2.qm \
    ap.o
