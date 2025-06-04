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
} // namespace TUI

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

		std::vector<std::shared_ptr<Polonius::TUI::Widget>> widgets;
		void drawWidgets();
		void updateBoundaries(std::shared_ptr<Polonius::TUI::Widget> widget);
		void resetBoundaries();

		void initialize();

	public:
		Window();
		~Window();
		int run();

		WINDOW* getScreen() const;

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
};



} // namespace Polonius

#endif // SRC_TUI_WINDOW_H_
