/**
 * Copyright (C) 2025 rail5
 */

#include "window.h"
#include "widget.h"

#include "../shared/version.h"

#include <iostream>
#include <signal.h>
#include <unistd.h>

extern Polonius::TUI::Window* mainWindow;
extern bool command_key_pressed;

Polonius::TUI::Window::Window() : screen(nullptr) {
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

	// Set up signal handlers
	signal(SIGTSTP, handleInterrupt);
	signal(SIGINT, handleInterrupt);

	Polonius::TUI::command_key_pressed = false;

	Polonius::TUI::mainWindow = this; // Set the main window pointer to this instance

	// Set up color pairs
	init_pair(Polonius::TUI::Color::RED, COLOR_WHITE, COLOR_RED); // Error color pair
	init_pair(Polonius::TUI::Color::GREEN, COLOR_WHITE, COLOR_GREEN); // Success color pair
	init_pair(Polonius::TUI::Color::YELLOW, COLOR_WHITE, COLOR_YELLOW); // Warning color pair
}

Polonius::TUI::Window::Window(Polonius::File* file) : Window() {
	this->file = file; // Set the file for this window
}

Polonius::TUI::Window::~Window() {
	endwin(); // End ncurses mode
}

WINDOW* Polonius::TUI::Window::getScreen() const {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}
	return screen;
}

Polonius::File* Polonius::TUI::Window::getFile() const {
	return file;
}

void Polonius::TUI::Window::refreshScreen() {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}
	endwin();
	refresh();
	clear();
	attroff(A_REVERSE);
	drawWidgets();
	move(cursor_y, cursor_x);
	// Reset any message on a screen refresh
	current_message.reset();
}

void Polonius::TUI::Window::clearScreen() {
	clear();
	refresh();
	attroff(A_REVERSE);
}

void Polonius::TUI::Window::setTop(int top) {
	if (top > LINES) {
		throw std::out_of_range("Top value is out of range");
	}
	this->top = top;
}

void Polonius::TUI::Window::setBottom(int bottom) {
	if (bottom > LINES) {
		throw std::out_of_range("Bottom value is out of range");
	}
	this->bottom = bottom;
}

void Polonius::TUI::Window::setLeft(int left) {
	if (left > COLS) {
		throw std::out_of_range("Left value is out of range");
	}
	this->left = left;
}

void Polonius::TUI::Window::setRight(int right) {
	if (right > COLS) {
		throw std::out_of_range("Right value is out of range");
	}
	this->right = right;
}

int Polonius::TUI::Window::getTop() const {
	return top;
}

int Polonius::TUI::Window::getBottom() const {
	return bottom;
}

int Polonius::TUI::Window::getLeft() const {
	return left;
}

int Polonius::TUI::Window::getRight() const {
	return right;
}

void Polonius::TUI::Window::initialize() {
	// Add the bottom pane of the window displaying some basic information
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}

	helpPane = std::make_shared<Polonius::TUI::HelpPane>(Polonius::TUI::BOTTOM, Polonius::TUI::FULL, 4, "Polonius v" program_version);
	if (file) {
		helpPane->setBottomLabel(file->getPath());
	} else {
		helpPane->setBottomLabel("New File");
	}

	textDisplay = std::make_shared<Polonius::TUI::TextDisplay>
		(Polonius::TUI::TOP, Polonius::TUI::FULL, Polonius::TUI::FULL);
	
	// Give the text display focus
	setFocus(textDisplay.get());

	initialized = true;
}

void Polonius::TUI::Window::drawWidgets() {
	if (!screen) {
		return; // Ensure screen is initialized
	}
	resetBoundaries(); // Reset boundaries before drawing widgets

	// First, draw the help pane at the bottom
	if (helpPane) {
		helpPane->setParent(this);
		helpPane->draw();
		updateBoundaries(helpPane); // Update boundaries based on the help pane's position
	}

	// Next, draw all the widgets
	for (auto& widget : widgets) {
		widget->setParent(this); // Set the parent window for the widget
		widget->draw();
		updateBoundaries(widget); // Update boundaries based on the widget's position
		if (focused_widget == widget.get()) {
			widget->takeCursor();
		}
	}

	// Finally, draw the text display widget after all others
	if (textDisplay) {
		textDisplay->setParent(this);
		textDisplay->draw();
		updateBoundaries(textDisplay);
		if (focused_widget == textDisplay.get()) {
			textDisplay->takeCursor(); // Ensure the text display has the cursor
		}
	}
}

void Polonius::TUI::Window::moveCursor(int x, int y) {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}

	cursor_x = std::min(x, COLS);
	cursor_y = std::min(y, LINES);
}

void Polonius::TUI::Window::moveCursor(const Polonius::TUI::Cursor& position) {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}

	cursor_x = std::min(position.x, COLS);
	cursor_y = std::min(position.y, LINES);
}

Polonius::TUI::Cursor Polonius::TUI::Window::getCursorPosition() const {
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}
	return {cursor_x, cursor_y};
}

int Polonius::TUI::Window::run() {
	if (!initialized) {
		initialize(); // Initialize the window and widgets if not already done
	}
	int ch;

	drawWidgets();

	// Move cursor to 0,0
	move(0, 0);

	while ((ch = getch())) {
		switch (ch) {
			case KEY_RESIZE:
				// Handle window resize
				refreshScreen();
				break;
			case KEY_NPAGE: // Page down
				textDisplay->pageDown();
				break;
			case KEY_PPAGE: // Page up
				textDisplay->pageUp();
				break;
			case 23: // Ctrl+W
				// If the search pane is up, shut it down
				if (!widgets.empty() && dynamic_cast<Polonius::TUI::SearchPane*>(widgets.back().get()) != nullptr) {
					widgets.pop_back();
					setFocus(textDisplay.get()); // Set focus back to the text display
				} else {
					// Otherwise, create a new search pane
					std::shared_ptr<Polonius::TUI::SearchPane> search_pane = std::make_shared<Polonius::TUI::SearchPane>
						(Polonius::TUI::BOTTOM, Polonius::TUI::FULL, 1);
					addWidget(search_pane);
					setFocus(search_pane.get()); // Set focus to the new search pane
				}
				refreshScreen();
				break;
			case 20: // Ctrl+T
				command_key_pressed = !command_key_pressed; // Toggle Ctrl+T state
				break;
			case 24: // Ctrl+X
				clear();
				return 0; // Exit the application
			default:
				// Pass the key to whichever widget has focus
				focused_widget->handleKeyPress(ch);
				break;
		}

		showMessage();

		move(cursor_y, cursor_x);
		// Highlight the current position
		attron(A_REVERSE);
	}

	clear();
	return 0;
}

void Polonius::TUI::Window::setFile(Polonius::File* file) {
	this->file = file;
}

void Polonius::TUI::Window::addWidget(std::shared_ptr<Polonius::TUI::Widget> widget) {
	widget->setParent(this);
	widgets.push_back(widget);
}

void Polonius::TUI::Window::updateBoundaries(std::shared_ptr<Polonius::TUI::Widget> widget) {
	// Update the top, bottom, left, and right boundaries if necessary
	// Only if the widget is positioned relatively
	if (!widget->isRelativelyPositioned()) {
		return; // No need to update boundaries for absolutely positioned widgets
	}
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
					top = (bottom - top) / 2;
					break;
				case Polonius::TUI::QUARTER:
					top = (bottom - top) / 4;
					break;
				case Polonius::TUI::THREE_QUARTERS:
					top = ((bottom - top) * 3) / 4;
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

void Polonius::TUI::Window::resetBoundaries() {
	// Reset boundaries to the initial state
	top = 0;
	bottom = LINES;
	left = 0;
	right = COLS;
}

void Polonius::TUI::Window::showMessage() {
	if (current_message) {
		current_message->draw();
	}
}

void Polonius::TUI::Window::restoreCursorPosition() {
	// Restore the cursor position to the last known position
	move(cursor_y, cursor_x);
	// Highlight the current position
	attron(A_REVERSE);
	refresh();
}

void Polonius::TUI::Window::setFocus(Polonius::TUI::Widget* widget) {
	if (!widget) {
		return;
	}
	focused_widget = widget;
	helpPane->setShortcuts(focused_widget->widgetShortcuts());
}

void Polonius::TUI::Window::setHelpPaneKeyShortcuts(const std::vector<Polonius::TUI::KeyShortcut>& shortcuts) {
	if (helpPane) {
		helpPane->setShortcuts(shortcuts);
	}
}

void Polonius::TUI::Window::handleInterrupt(int signal) {
	// Handle interrupts (e.g., Ctrl+C)
	switch (signal) {
		case SIGINT: // Ctrl+C
			break;
		case SIGTSTP: // Ctrl+Z
			// Do not stop the application on Ctrl+Z
			// Ctrl+T + Ctrl+Z will suspend the application instead
			if (!Polonius::TUI::mainWindow) {
				// No window is initialized, just suspend
				kill(getpid(), SIGSTOP); // Suspend the process
				return;
			}
			if (Polonius::TUI::command_key_pressed) {
				Polonius::TUI::command_key_pressed = false;
				Polonius::TUI::mainWindow->clearScreen();
				kill(getpid(), SIGSTOP); // Suspend the process
				// After resuming:
				Polonius::TUI::mainWindow->refreshScreen();
				Polonius::TUI::mainWindow->restoreCursorPosition();
			} else {
				// Draw a message indicating that Ctrl+T is required to suspend
				std::shared_ptr<Polonius::TUI::Message> msg = std::make_shared<Polonius::TUI::Message>();
				msg->setMessage("[ To suspend, type ^T^Z ]");
				msg->setColor(Polonius::TUI::RED);
				msg->setParent(Polonius::TUI::mainWindow);
				Polonius::TUI::mainWindow->current_message = msg;
				msg->draw();
			}
			break;
	}
}
