/**
 * Copyright (C) 2025 rail5
 */

#include <string>
#include "widget.h"

Polonius::TUI::Widget::Widget() = default;

Polonius::TUI::Widget::Widget(int x, int y, int w, int h)
	: x_(x), y_(y), w_(w), h_(h),
	position(Polonius::TUI::ABSOLUTE) {}

Polonius::TUI::Widget::Widget(Polonius::TUI::Edge anchor, int width, int height)
	: x_(0), y_(0), w_(width), h_(height),
	position(Polonius::TUI::RELATIVE), anchor(anchor) {}

Polonius::TUI::Widget::~Widget() = default;

/**
 * @brief Draw the widget to the screen
 * 
 * The default implementation does nothing.
 * Derived classes should override this method to provide their own drawing logic.
 */
void Polonius::TUI::Widget::draw() {}

WINDOW* Polonius::TUI::Widget::getSubwindow() {
	// Calculate positioning
	if (!parent) {
		throw std::runtime_error("Parent window is not set for the widget");
	}
	// Get the width and height
	int x = x_;
	int y = y_;

	int available_width = parent->getRight() - parent->getLeft();
	int available_height = parent->getBottom() - parent->getTop();

	switch (w_) {
		case Polonius::TUI::FULL:
			width = available_width; // Full width
			break;
		case Polonius::TUI::HALF:
			width = available_width / 2; // Half width
			break;
		case Polonius::TUI::QUARTER:
			width = available_width / 4; // Quarter width
			break;
		case Polonius::TUI::THREE_QUARTERS:
			width = (available_width * 3) / 4; // Three-quarters width
			break;
		default:
			width = w_ <= 0 ? available_width : w_; // Use the specified width or default to full width
			break;
	}

	switch (h_) {
		case Polonius::TUI::FULL:
			height = available_height; // Full height
			break;
		case Polonius::TUI::HALF:
			height = available_height / 2; // Half height
			break;
		case Polonius::TUI::QUARTER:
			height = available_height / 4; // Quarter height
			break;
		case Polonius::TUI::THREE_QUARTERS:
			height = (available_height * 3) / 4; // Three-quarters height
			break;
		default:
			height = h_ <= 0 ? available_height : h_; // Use the specified height or default to full height
			break;
	}

	// Are we relatively or absolutely positioned?
	if (position == Polonius::TUI::RELATIVE) {
		// If relative, we need to calculate the position based on the anchor and width/height
		switch (anchor) {
			case Polonius::TUI::LEFT:
				x = parent->getLeft(); // Left edge
				y = (available_height - height) / 2; // Center vertically
				break;
			case Polonius::TUI::RIGHT:
				x = parent->getRight() - width; // Right edge
				y = (available_height - height) / 2; // Center vertically
				break;
			case Polonius::TUI::TOP:
				x = (available_width - width) / 2; // Center horizontally
				y = parent->getTop(); // Top edge
				break;
			case Polonius::TUI::BOTTOM:
				x = (available_width - width) / 2; // Center horizontally
				y = parent->getBottom() - height; // Bottom edge
				break;
		}
	}

	return subwin(parent->getScreen(), height, width, y, x);
}

void Polonius::TUI::Widget::setParent(Polonius::TUI::Window* parent) {
	this->parent = parent;
}

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

Polonius::TUI::Edge Polonius::TUI::Widget::getAnchor() const {
	return anchor;
}

bool Polonius::TUI::Widget::isRelativelyPositioned() const {
	return position == Polonius::TUI::RELATIVE;
}

void Polonius::TUI::Widget::setX(int x) {
	x_ = x;
}

void Polonius::TUI::Widget::setY(int y) {
	y_ = y;
}

/**
 * @brief Set the width of the widget
 * 
 * You can set the width to a specific value, or use the predefined constants:
 * - Polonius::TUI::FULL for full width
 * - Polonius::TUI::HALF for half width
 * - Polonius::TUI::QUARTER for quarter width
 * - Polonius::TUI::THREE_QUARTERS for three-quarters width
 * 
 * These predefined constants will be automatically recalculated when the window is resized.
 */
void Polonius::TUI::Widget::setWidth(int w) {
	w_ = w;
}

/**
 * @brief Set the height of the widget
 * 
 * You can set the height to a specific value, or use the predefined constants:
 * - Polonius::TUI::FULL for full height
 * - Polonius::TUI::HALF for half height
 * - Polonius::TUI::QUARTER for quarter height
 * - Polonius::TUI::THREE_QUARTERS for three-quarters height
 * 
 * These predefined constants will be automatically recalculated when the window is resized.
 */
void Polonius::TUI::Widget::setHeight(int h) {
	h_ = h;
}

/**
 * @brief Set positioning (absolute or relative) of the widget
 * 
 * You can set the positioning to either:
 * - Polonius::TUI::RELATIVE for relative positioning
 * - Polonius::TUI::ABSOLUTE for absolute positioning
 */
void Polonius::TUI::Widget::setPositioning(Polonius::TUI::Positioning pos) {
	position = pos;
}

/**
 * @brief Handle keypresses sent to the widget
 * 
 * The default implementation ignores all keypresses
 */
void Polonius::TUI::Widget::handleKeyPress(int ch) {}


const std::vector<Polonius::TUI::KeyShortcut> Polonius::TUI::Widget::widgetShortcuts() const {
	return {};
}
