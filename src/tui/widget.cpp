/**
 * Copyright (C) 2025 rail5
 */

#include <string>
#include <array>
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

Polonius::TUI::HelpPane::HelpPane(int x, int y, int w, int h, const std::string& label)
	: Widget(x, y, w, h), topLabel(label) {}

void Polonius::TUI::HelpPane::setBottomLabel(const std::string& label) {
	bottomLabel = label;
}

void Polonius::TUI::HelpPane::draw(WINDOW* window) {
	// Draw a box at the specified position with the given width and height
	WINDOW* pane = subwin(window, h_, w_, y_, x_);

	if (pane == nullptr) {
		return;
	}

	box(pane, 0, 0); // Draw a box at the specified position with the given width and height

	// Print the label in the top-center of the pane
	int label_x = (w_ - static_cast<int>(topLabel.length())) / 2; // Center the label
	mvwprintw(pane, 0, label_x, "%s", topLabel.c_str()); // Print the label at the top of the pane

	// Print the bottom label in the bottom-center of the pane
	if (!bottomLabel.empty()) {
		int bottom_label_x = (w_ - static_cast<int>(bottomLabel.length())) / 2; // Center the bottom label
		mvwprintw(pane, h_ - 1, bottom_label_x, "%s", bottomLabel.c_str()); // Print the bottom label
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
	int num_rows = h_ / 2;

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
		int x_position = (col - 1) * (w_ / num_columns) + 1;
		
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
