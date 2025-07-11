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
	// Draw a box at the specified position with the given width and height
	WINDOW* pane = getSubwindow();

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

	int num_shortcuts = static_cast<int>(shortcuts.size());
	int num_columns = (num_shortcuts / 2) + (num_shortcuts % 2);
	if (num_columns < 1) {
		num_columns = 1;
	}
	int num_rows = height / 2;

	int row = 0;
	int col = 0;

	/**
	 * Print the shortcuts in a grid-like format
	 * Row 1, col 1
	 * Then row 2, col 1
	 * Then row 1, col 2
	 * Then row 2, col 2
	 * And so on
	 * 
	 * Briefly:
	 * 		The row number == i % 2
	 * 		The column number == ceil([i+1] / 2)
	 */

	for (size_t i = 0; i < shortcuts.size(); i++) {
		// Calculate row and column number
		row++;
		if (row > num_rows) {
			row = 1; // Reset row to 1 if it exceeds the number of rows
			col++; // Move to the next column
		}

		// Calculate the actual x position based on the column number
		// Divide the width by the number of columns
		int x_position = col * (width / num_columns) + 1;
		
		// Print the key combination with a standout (or bold) attribute
		wattron(pane, A_STANDOUT);
		mvwprintw(pane, row, x_position, "%.*s", 2, shortcuts[i].key.c_str());
		wattroff(pane, A_STANDOUT);

		// Print the description normally
		mvwprintw(pane, row, x_position + 3, "%s", shortcuts[i].description.c_str());
	}

	wrefresh(pane); // Refresh the pane to show the changes
	delwin(pane); // Delete the pane window after drawing
}

void Polonius::TUI::HelpPane::setShortcuts(const std::vector<Polonius::TUI::KeyShortcut>& newShortcuts) {
	shortcuts = newShortcuts;
}
