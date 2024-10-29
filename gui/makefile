# Qt settings
QT_INCLUDES = `pkg-config --cflags Qt6Core Qt6Gui Qt6Widgets`
QT_LIBS = `pkg-config --libs Qt6Core Qt6Gui Qt6Widgets`

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=gnu++20 -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

all:
	$(CXX) $(CXXFLAGS) $(QT_INCLUDES) -o bin/polonius-gui src/main.cpp $(QT_LIBS)

clean:
	rm -f $(TARGET)

