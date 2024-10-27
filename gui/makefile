# Detect operating system
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    TARGET_EXT = .exe
    QTDIR = C:/src/Qt/6.8.0/mingw_64
    MKDIR = if not exist "$(1)" md "$(1)"
    RM = if exist "$(1)" rd /s /q "$(1)"
    PATHSEP = \\
else
    PLATFORM = unix
    TARGET_EXT =
    QTDIR = /usr/lib/qt6
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
    PATHSEP = /
endif

# Qt settings
QT_INCLUDES = -I$(QTDIR)/include \
             -I$(QTDIR)/include/QtCore \
             -I$(QTDIR)/include/QtGui \
             -I$(QTDIR)/include/QtWidgets
QT_LIBS = -L$(QTDIR)/lib -lQt6Widgets -lQt6Core -lQt6Gui

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++17 -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

# Directory structure
SRC_DIR = src
BUILD_DIR = build$(PATHSEP)$(PLATFORM)
OBJ_DIR = $(BUILD_DIR)$(PATHSEP)obj

# Project files
SOURCES = $(SRC_DIR)/main.cpp
HEADERS = $(SRC_DIR)/main.h
TARGET = $(BUILD_DIR)$(PATHSEP)editor$(TARGET_EXT)

# Generated files
MOC_CPP = $(OBJ_DIR)$(PATHSEP)moc_main.cpp
OBJECTS = $(OBJ_DIR)$(PATHSEP)main.o $(MOC_CPP:.cpp=.o)

# Build rules
all: create_dirs $(TARGET)

# Create directories
create_dirs:
	@$(call MKDIR,$(BUILD_DIR))
	@$(call MKDIR,$(OBJ_DIR))

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(QT_LIBS)

$(OBJ_DIR)$(PATHSEP)main.o: $(SRC_DIR)/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(QT_INCLUDES) -c $(SRC_DIR)/main.cpp -o $@

$(MOC_CPP): $(HEADERS)
	moc $(HEADERS) -o $(MOC_CPP)

$(OBJ_DIR)$(PATHSEP)moc_main.o: $(MOC_CPP)
	$(CXX) $(CXXFLAGS) $(QT_INCLUDES) -c $(MOC_CPP) -o $@

clean:
	@$(call RM,$(BUILD_DIR))

.PHONY: all clean create_dirs