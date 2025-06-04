/**
 * Copyright (C) 2025 rail5
 */

#include <string>
#include "widget.h"

Polonius::TUI::Widget::Widget(int x, int y, int w, int h) : x_(x), y_(y), w_(w), h_(h) {}
Polonius::TUI::Widget::~Widget() = default;

int Polonius::TUI::Widget::getX() const {
	return x_;
}

int Polonius::TUI::Widget::getY() const {
	return y_;
}

int Polonius::TUI::Widget::getWidth() const {
	return w_;
}

int Polonius::TUI::Widget::getHeight() const {
	return h_;
}

Polonius::TUI::Pane::Pane(int x, int y, int w, int h, const std::string& label)
	: Widget(x, y, w, h), label(label) {}

void Polonius::TUI::Pane::draw(WINDOW* window) {
	// Draw a box at the specified position with the given width and height
	WINDOW* pane = subwin(window, h_, w_, y_, x_);

	if (pane == nullptr) {
		return;
	}

	box(pane, 0, 0); // Draw a box at the specified position with the given width and height
	mvwprintw(pane, 0, 1, "%s", label.c_str()); // Print the label at the top of the pane
	wrefresh(pane); // Refresh the pane to show the changes
	delwin(pane); // Delete the pane window after drawing
}
