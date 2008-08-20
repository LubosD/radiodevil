TEMPLATE = app
TARGET = radiodevil
CONFIG += qt debug

SOURCES += radiodevil.cpp \
 RadioDevice.cpp \
 AlsaMixer.cpp \
 SoxReader.cpp \
 SoxWriter.cpp \
 MainWindow.cpp \
 Settings.cpp \
 FrequencyDisplay.cpp \
 SettingsDlg.cpp \
 Preset.cpp \
 ContextTreeWidget.cpp

HEADERS += \
RadioDevice.h \
GeneralException.h \
 MainWindow.h \
 SettingsDlg.h \
 ContextTreeWidget.h

FORMS += MainWindow.ui SettingsDlg.ui ListNameDlg.ui
RESOURCES += gfx/resources.qrc

LIBS += -lasound -lsox
