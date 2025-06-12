/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_TUI_WINDOW_H_
#define SRC_TUI_WINDOW_H_

#include "../polonius.h"
#include <ncurses.h>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace Polonius {

// Forward declarations
namespace TUI {
class Widget;
class TextDisplay;
class HelpPane;
class Message;

enum Color : uint8_t;

class Window;

[[maybe_unused]] static Polonius::TUI::Window* mainWindow;
[[maybe_unused]] static bool command_key_pressed = false;

struct Cursor {
	int x = 0;
	int y = 0;
};

/**
 * @class Window
 * @brief Represents a window in the Terminal UI
 * 
 * This UI is done with ncurses
 */
class Window {
	private:
		WINDOW* screen;
		Polonius::File* file;
		std::shared_ptr<Polonius::TUI::TextDisplay> textDisplay;
		int width = 80;
		int height = 24;
		int top = 0;
		int bottom = 24;
		int left = 0;
		int right = 80;
		bool initialized = false;

		int cursor_x = 0;
		int cursor_y = 0;

		Polonius::TUI::Widget* focused_widget = nullptr;

		std::vector<std::shared_ptr<Polonius::TUI::Widget>> widgets;
		std::shared_ptr<Polonius::TUI::Message> current_message;
		void drawWidgets();
		void updateBoundaries(std::shared_ptr<Polonius::TUI::Widget> widget);
		void resetBoundaries();

		void initialize();

	public:
		Window();
		explicit Window(Polonius::File* file);
		~Window();
		int run();

		WINDOW* getScreen() const;
		Polonius::File* getFile() const;

		void refreshScreen();
		void clearScreen();

		void setTop(int top);
		void setBottom(int bottom);
		void setLeft(int left);
		void setRight(int right);

		int getTop() const;
		int getBottom() const;
		int getLeft() const;
		int getRight() const;

		void setFile(Polonius::File* file);

		void addWidget(std::shared_ptr<Polonius::TUI::Widget> widget);
		void showMessage();

		void restoreCursorPosition();
		void moveCursor(int x, int y);

		Polonius::TUI::Cursor getCursorPosition() const;

		void setFocus(Polonius::TUI::Widget* widget);

		// Interrupt handling
		static void handleInterrupt(int signal);
};
} // namespace TUI
} // namespace Polonius

#endif // SRC_TUI_WINDOW_H_
