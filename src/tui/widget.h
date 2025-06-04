/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_TUI_WIDGET_H_
#define SRC_TUI_WIDGET_H_

#include <ncurses.h>
#include <string>

namespace Polonius {
namespace TUI {


enum Positioning {
	RELATIVE,
	ABSOLUTE
};

enum Edge {
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

enum RelativeSize : int {
	FULL = -1,
	HALF = -2,
	QUARTER = -3,
	THREE_QUARTERS = -4
};

class Widget {
	protected:
		int x_;
		int y_;
		int w_;
		int h_;
		Polonius::TUI::Positioning position = Polonius::TUI::RELATIVE;
		Polonius::TUI::Edge anchor = Polonius::TUI::BOTTOM;

	public:
		Widget(int x, int y, int w, int h);
		Widget(Polonius::TUI::Edge anchor, int width, int height);

		virtual ~Widget();

		virtual void draw(WINDOW* window) = 0; // Pure virtual function for drawing the widget

		int getX() const;
		int getY() const;
		int getWidth() const;
		int getHeight() const;
};

class HelpPane : public Widget {
	private:
		std::string topLabel;
		std::string bottomLabel;
	public:
		HelpPane(int x, int y, int w, int h, const std::string& label);
		HelpPane(Polonius::TUI::Edge anchor, int width, int height, const std::string& label);
		void setBottomLabel(const std::string& label);

		void draw(WINDOW* window) override;
};

} // namespace TUI
} // namespace Polonius

#endif // SRC_TUI_WIDGET_H_
