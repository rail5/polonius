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

#include "widget.h"

namespace Polonius {

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
		std::string buffer;
		unsigned int width = 80;
		unsigned int height = 24;

		std::vector<std::shared_ptr<Polonius::TUI::Widget>> widgets;
		void drawWidgets();

		void initialize();

	public:
		Window();
		~Window();
		int run();

		void setFile(Polonius::File* file);

		void addWidget(std::shared_ptr<Polonius::TUI::Widget> widget);
};



} // namespace Polonius

#endif // SRC_TUI_WINDOW_H_
