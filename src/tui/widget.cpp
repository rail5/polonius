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
