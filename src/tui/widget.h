/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_TUI_WIDGET_H_
#define SRC_TUI_WIDGET_H_

#include <ncurses.h>
#include <string>

namespace Polonius {
namespace TUI {


class Widget {
	protected:
		int x_;
		int y_;
		int w_;
		int h_;

	public:
		Widget(int x, int y, int w, int h);

		virtual ~Widget();

		virtual void draw(WINDOW* window) = 0; // Pure virtual function for drawing the widget

		int getX() const;
		int getY() const;
		int getWidth() const;
		int getHeight() const;
};

class Pane : public Widget {
	private:
		std::string label;
	public:
		Pane(int x, int y, int w, int h, const std::string& label);

		void draw(WINDOW* window) override;
};

} // namespace TUI
} // namespace Polonius

#endif // SRC_TUI_WIDGET_H_
