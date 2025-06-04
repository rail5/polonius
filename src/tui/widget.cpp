/**
 * Copyright (C) 2025 rail5
 */

#include <string>
#include <array>
#include "widget.h"

Polonius::TUI::Widget::Widget() = default;

Polonius::TUI::Widget::Widget(int x, int y, int w, int h)
	: x_(x), y_(y), w_(w), h_(h),
	position(Polonius::TUI::ABSOLUTE) {}

Polonius::TUI::Widget::Widget(Polonius::TUI::Edge anchor, int width, int height)
	: x_(0), y_(0), w_(width), h_(height),
	position(Polonius::TUI::RELATIVE), anchor(anchor) {}

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

Polonius::TUI::HelpPane::HelpPane(int x, int y, int w, int h, const std::string& label)
	: Widget(x, y, w, h), topLabel(label) {}

Polonius::TUI::HelpPane::HelpPane(Polonius::TUI::Edge anchor, int width, int height, const std::string& label)
	: Widget(anchor, width, height), topLabel(label) {}

void Polonius::TUI::HelpPane::setBottomLabel(const std::string& label) {
	bottomLabel = label;
}

void Polonius::TUI::HelpPane::draw(WINDOW* window) {
	// Get the width and height
	int width, height;
	int x = x_;
	int y = y_;

	switch (w_) {
		case Polonius::TUI::FULL:
			width = COLS; // Full width
			break;
		case Polonius::TUI::HALF:
			width = COLS / 2; // Half width
			break;
		case Polonius::TUI::QUARTER:
			width = COLS / 4; // Quarter width
			break;
		case Polonius::TUI::THREE_QUARTERS:
			width = (COLS * 3) / 4; // Three-quarters width
			break;
		default:
			width = w_ <= 0 ? COLS : w_; // Use the specified width or default to COLS
			break;
	}

	switch (h_) {
		case Polonius::TUI::FULL:
			height = LINES; // Full height
			break;
		case Polonius::TUI::HALF:
			height = LINES / 2; // Half height
			break;
		case Polonius::TUI::QUARTER:
			height = LINES / 4; // Quarter height
			break;
		case Polonius::TUI::THREE_QUARTERS:
			height = (LINES * 3) / 4; // Three-quarters height
			break;
		default:
			height = h_ <= 0 ? LINES : h_; // Use the specified height or default to LINES
			break;
	}

	// Are we relatively or absolutely positioned?
	if (position == Polonius::TUI::RELATIVE) {
		// If relative, we need to calculate the position based on the anchor and width/height
		switch (anchor) {
			case Polonius::TUI::LEFT:
				x = 0; // Left edge
				y = (LINES - height) / 2; // Center vertically
				break;
			case Polonius::TUI::RIGHT:
				x = COLS - width; // Right edge
				y = (LINES - height) / 2; // Center vertically
				break;
			case Polonius::TUI::TOP:
				x = (COLS - width) / 2; // Center horizontally
				y = 0; // Top edge
				break;
			case Polonius::TUI::BOTTOM:
				x = (COLS - width) / 2; // Center horizontally
				y = LINES - height; // Bottom edge
				break;
		}
	}

	// Draw a box at the specified position with the given width and height
	WINDOW* pane = subwin(window, height, width, y, x);

	if (pane == nullptr) {
		return;
	}

	box(pane, 0, 0); // Draw a box at the specified position with the given width and height

	// Print the label in the top-center of the pane
	if (!topLabel.empty()) {
		// If the label is too long, truncate it to fit
		// Do so by showing the beginning of the label, then ellipses, then the end of the label
		auto maxContent = static_cast<std::string::size_type>(width - 2); // -2 for the box borders
		if (topLabel.length() > maxContent) {
			auto trim = static_cast<std::string::size_type>((width - 5) / 2);
			std::string truncatedLabel = topLabel.substr(0, trim) + "..." + topLabel.substr(topLabel.length() - trim);
			topLabel = truncatedLabel;
		}
		int label_x = (width - static_cast<int>(topLabel.length())) / 2; // Center the label
		mvwprintw(pane, 0, label_x, "%s", topLabel.c_str()); // Print the label at the top of the pane
	}

	// Print the bottom label in the bottom-center of the pane
	if (!bottomLabel.empty()) {
		// If the bottom label is too long, truncate it to fit
		// Do so by showing the beginning of the label, then ellipses, then the end of the label
		auto maxContent = static_cast<std::string::size_type>(width - 2); // -2 for the box borders
		if (bottomLabel.length() > maxContent) {
			auto trim = static_cast<std::string::size_type>((width - 5) / 2);
			std::string truncatedLabel = bottomLabel.substr(0, trim) + "..." + bottomLabel.substr(bottomLabel.length() - trim);
			bottomLabel = truncatedLabel;
		}
		int bottom_label_x = (width - static_cast<int>(bottomLabel.length())) / 2; // Center the bottom label
		mvwprintw(pane, height - 1, bottom_label_x, "%s", bottomLabel.c_str()); // Print the bottom label
	}

	// Place some helpful text in the pane
	// Describing keyboard shortcuts
	constexpr std::array<const char*, 6> shortcuts = {
		"^S Save",
		"^K Cut",
		"^W Where Is",
		"^X Exit",
		"^U Paste",
		"^/ Go to line"
	};

	int num_columns = std::max(shortcuts.size() / 2, static_cast<size_t>(1));
	int num_rows = height / 2;

	int row = 1;
	int col = 1;

	for (size_t i = 0; i < shortcuts.size(); i++) {
		// Calculate row and column number
		// We should print like this:
		// Row 1, column 1
		// Then row 1, column 2
		// Etc to the max column number
		// Then row 2, column 1
		// And so on
		if (col > num_columns) {
			col = 1; // Reset column to 1
			row++; // Move to the next row
		}
		if (row > num_rows) {
			break; // Stop if we exceed the number of rows
		}

		// Calculate the actual x position based on the column number
		// Divide the width by the number of columns
		int x_position = (col - 1) * (width / num_columns) + 1;
		
		// Print the first two chars with a standout (or bold) attribute
		wattron(pane, A_STANDOUT);
		mvwprintw(pane, row, x_position, "%.*s", 2, shortcuts[i]);
		wattroff(pane, A_STANDOUT);

		// print the rest normally
		mvwprintw(pane, row, x_position + 2, "%s", shortcuts[i] + 2);
		col++;
	}

	wrefresh(pane); // Refresh the pane to show the changes
	delwin(pane); // Delete the pane window after drawing
}
