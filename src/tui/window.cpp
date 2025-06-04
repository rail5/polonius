/**
 * Copyright (C) 2025 rail5
 */

#include "window.h"
#include "widget.h"

#include <iostream>

Polonius::Window::Window() : screen(nullptr) {
	initscr(); // Initialize ncurses
	cbreak(); // Disable line buffering
	noecho(); // Don't echo input characters
	keypad(stdscr, TRUE); // Enable special keys
	start_color(); // Start color functionality
	use_default_colors(); // Use default terminal colors
	screen = stdscr;

	width = COLS; // Set default width to terminal width
	height = LINES; // Set default height to terminal height
	top = 0; // Set default top to 0
	bottom = LINES; // Set default bottom to last line
	left = 0; // Set default left to 0
	right = COLS; // Set default right to last column
}

Polonius::Window::~Window() {
	endwin(); // End ncurses mode
}

WINDOW* Polonius::Window::getScreen() const {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}
	return screen;
}

void Polonius::Window::setTop(int top) {
	if (top > LINES) {
		throw std::out_of_range("Top value is out of range");
	}
	this->top = top;
}

void Polonius::Window::setBottom(int bottom) {
	if (bottom > LINES) {
		throw std::out_of_range("Bottom value is out of range");
	}
	this->bottom = bottom;
}

void Polonius::Window::setLeft(int left) {
	if (left > COLS) {
		throw std::out_of_range("Left value is out of range");
	}
	this->left = left;
}

void Polonius::Window::setRight(int right) {
	if (right > COLS) {
		throw std::out_of_range("Right value is out of range");
	}
	this->right = right;
}

int Polonius::Window::getTop() const {
	return top;
}

int Polonius::Window::getBottom() const {
	return bottom;
}

int Polonius::Window::getLeft() const {
	return left;
}

int Polonius::Window::getRight() const {
	return right;
}

void Polonius::Window::initialize() {
	// Add the bottom pane of the window displaying some basic information
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}

	std::shared_ptr<Polonius::TUI::HelpPane> bottom_pane = std::make_shared<Polonius::TUI::HelpPane>
		(Polonius::TUI::BOTTOM, Polonius::TUI::FULL, 4, "Polonius");
	bottom_pane->setBottomLabel("New File");
	widgets.push_back(bottom_pane);

	initialized = true;
}

void Polonius::Window::drawWidgets() {
	if (!screen) {
		return; // Ensure screen is initialized
	}
	resetBoundaries(); // Reset boundaries before drawing widgets
	for (auto& widget : widgets) {
		widget->setParent(this); // Set the parent window for the widget
		widget->draw();
		updateBoundaries(widget); // Update boundaries based on the widget's position
	}
}

int Polonius::Window::run() {
	if (!initialized) {
		initialize(); // Initialize the window and widgets if not already done
	}
	int ch;
	int x = 0, y = 0;

	drawWidgets();

	// Move cursor to 0,0
	move(0, 0);

	while ((ch = getch())) {
		erase();
		attroff(A_REVERSE); // Turn off reverse attribute
		drawWidgets(); // Redraw widgets on each key press
		refresh();

		switch (ch) {
			case KEY_RESIZE:
				// Handle window resize
				endwin();
				refresh();
				clear();
				drawWidgets();
				move(y, x); // Move cursor to the current position after resize
				break;
			case KEY_UP:
				if (y > 0) y--;
				break;
			case KEY_DOWN:
				if (y < LINES - 1) y++;
				break;
			case KEY_LEFT:
				if (x > 0) x--;
				break;
			case KEY_RIGHT:
				if (x < COLS - 1) x++;
				break;
			case 24: // Ctrl+X
				clear();
				return 0; // Exit the application
			default:
				// Handle other keys or input
				buffer += static_cast<char>(ch);
				break;
		}
		move(y, x);
		// Highlight the current position
		attron(A_REVERSE);
	}

	clear();
	return 0;
}

void Polonius::Window::setFile(Polonius::File* file) {
	this->file = file;

	// Populate the initial buffer
	if (file) {
		buffer = file->readFromFile(0, static_cast<int64_t>(Polonius::block_size));
	}
}

void Polonius::Window::addWidget(std::shared_ptr<Polonius::TUI::Widget> widget) {
	widgets.push_back(widget);
}

void Polonius::Window::updateBoundaries(std::shared_ptr<Polonius::TUI::Widget> widget) {
	// Update the top, bottom, left, and right boundaries if necessary
	// Only if the widget is positioned relatively
	if (widget->isRelativelyPositioned()) {
		switch (widget->getAnchor()) {
			case Polonius::TUI::LEFT:
				switch (widget->getWidth()) {
					case Polonius::TUI::FULL:
						left = 0;
						right = 0;
						break;
					case Polonius::TUI::HALF:
						left /= 2;
						break;
					case Polonius::TUI::QUARTER:
						left /= 4;
						break;
					case Polonius::TUI::THREE_QUARTERS:
						left = (left * 3) / 4;
						break;
					default:
						left += widget->getWidth();
						if (left > COLS) {
							left = COLS; // Ensure left does not exceed screen width
						}
						break;
				}
				break;
			case Polonius::TUI::RIGHT:
				switch (widget->getWidth()) {
					case Polonius::TUI::FULL:
						right = 0;
						left = 0;
						break;
					case Polonius::TUI::HALF:
						right /= 2;
						break;
					case Polonius::TUI::QUARTER:
						right /= 4;
						break;
					case Polonius::TUI::THREE_QUARTERS:
						right = (right * 3) / 4;
						break;
					default:
						right -= widget->getWidth();
						if (right < left) {
							right = left; // Ensure right does not exceed left
						}
						break;
				}
				break;
			case Polonius::TUI::TOP:
				switch (widget->getHeight()) {
					case Polonius::TUI::FULL:
						top = 0;
						bottom = 0;
						break;
					case Polonius::TUI::HALF:
						top /= 2;
						break;
					case Polonius::TUI::QUARTER:
						top /= 4;
						break;
					case Polonius::TUI::THREE_QUARTERS:
						top = (top * 3) / 4;
						break;
					default:
						top += widget->getHeight();
						if (top > LINES) {
							top = LINES; // Ensure top does not exceed screen height
						}
						break;
				}
				break;
			case Polonius::TUI::BOTTOM:
				switch (widget->getHeight()) {
					case Polonius::TUI::FULL:
						bottom = 0;
						top = 0;
						break;
					case Polonius::TUI::HALF:
						bottom /= 2;
						break;
					case Polonius::TUI::QUARTER:
						bottom /= 4;
						break;
					case Polonius::TUI::THREE_QUARTERS:
						bottom = (bottom * 3) / 4;
						break;
					default:
						bottom -= widget->getHeight();
						if (bottom < top) {
							bottom = top; // Ensure bottom does not exceed top
						}
						break;
				}
				break;
		}
	}
}

void Polonius::Window::resetBoundaries() {
	// Reset boundaries to the initial state
	top = 0;
	bottom = LINES;
	left = 0;
	right = COLS;
}
