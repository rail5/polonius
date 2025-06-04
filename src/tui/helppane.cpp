/**
 * Copyright (C) 2025 rail5
 */

#include "widget.h"
#include <array>

Polonius::TUI::HelpPane::HelpPane(int x, int y, int w, int h, const std::string& label)
	: Widget(x, y, w, h), topLabel(label) {}

Polonius::TUI::HelpPane::HelpPane(Polonius::TUI::Edge anchor, int width, int height, const std::string& label)
	: Widget(anchor, width, height), topLabel(label) {}

void Polonius::TUI::HelpPane::setBottomLabel(const std::string& label) {
	bottomLabel = label;
}

void Polonius::TUI::HelpPane::draw() {
	// Get the width and height
	int width, height;
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
				x = 0; // Left edge
				y = (available_height - height) / 2; // Center vertically
				break;
			case Polonius::TUI::RIGHT:
				x = available_width - width; // Right edge
				y = (available_height - height) / 2; // Center vertically
				break;
			case Polonius::TUI::TOP:
				x = (available_width - width) / 2; // Center horizontally
				y = 0; // Top edge
				break;
			case Polonius::TUI::BOTTOM:
				x = (available_width - width) / 2; // Center horizontally
				y = available_height - height; // Bottom edge
				break;
		}
	}

	// Draw a box at the specified position with the given width and height
	WINDOW* pane = subwin(parent->getScreen(), height, width, y, x);

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
