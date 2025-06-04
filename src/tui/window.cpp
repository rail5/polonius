/**
 * Copyright (C) 2025 rail5
 */

#include "window.h"
#include "widget.h"

#include "../shared/version.h"

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

void Polonius::Window::refreshScreen() {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}
	endwin();
	refresh();
	clear();
	attroff(A_REVERSE);
	drawWidgets();
	move(cursor_y, cursor_x);
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
		(Polonius::TUI::BOTTOM, Polonius::TUI::FULL, 4, "Polonius v" program_version);

	if (file) {
		bottom_pane->setBottomLabel(file->getPath());
	} else {
		bottom_pane->setBottomLabel("New File");
	}

	widgets.push_back(bottom_pane);

	textDisplay = std::make_shared<Polonius::TUI::TextDisplay>
		(Polonius::TUI::TOP, Polonius::TUI::FULL, Polonius::TUI::FULL);
	textDisplay->setBuffer(file->readFromFile(0, static_cast<int64_t>(Polonius::block_size)));

	widgets.push_back(textDisplay);

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

	drawWidgets();

	// Move cursor to 0,0
	move(0, 0);

	while ((ch = getch())) {
		refreshScreen();

		switch (ch) {
			case KEY_RESIZE:
				// Handle window resize
				refreshScreen();
				break;
			case KEY_UP:
				if (cursor_y > textDisplay->getTopEdge()) {
					cursor_y--;
				} else {
					textDisplay->scrollUp();
				}
				break;
			case KEY_DOWN:
				if (cursor_y < textDisplay->getBottomEdge()) {
					cursor_y++;
				} else {
					textDisplay->scrollDown();
				}
				break;
			case KEY_LEFT:
				if (cursor_x > textDisplay->getLeftEdge()) {
					cursor_x--;
				} else if (cursor_y > textDisplay->getTopEdge()) {
					// Move up one line, and land at the rightmost position
					cursor_y--;
					cursor_x = textDisplay->getRightEdge(); // Move to the rightmost position of the previous line
				}
				break;
			case KEY_RIGHT:
				//if (cursor_x < COLS - 1) cursor_x++;
				if (cursor_x < textDisplay->getRightEdge()) {
					cursor_x++;
				} else if (cursor_y < textDisplay->getBottomEdge()) {
					// Move down one line, and land at the leftmost position
					cursor_y++;
					cursor_x = textDisplay->getLeftEdge(); // Move to the leftmost position of the next line
				}
				break;
			case 24: // Ctrl+X
				clear();
				return 0; // Exit the application
			default:
				// Handle other keys or input
				//buffer += static_cast<char>(ch);
				break;
		}
		move(cursor_y, cursor_x);
		// Highlight the current position
		attron(A_REVERSE);
	}

	clear();
	return 0;
}

void Polonius::Window::setFile(Polonius::File* file) {
	this->file = file;
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
