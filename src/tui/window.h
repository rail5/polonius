/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_TUI_WINDOW_H_
#define SRC_TUI_WINDOW_H_

#include "../polonius.h"
#include <cursesapp.h>
#include <cursesw.h>
#include <string>
#include <cstdint>

namespace Polonius {

/**
 * @class Window
 * @brief Represents a window in the Terminal UI
 * 
 * This UI is done with ncurses
 */
class Window : public NCursesApplication {
	private:
		NCursesWindow* screen;
		std::string buffer;

	public:
		Window();
		~Window();
		int run() override;
};

} // namespace Polonius

#endif // SRC_TUI_WINDOW_H_
