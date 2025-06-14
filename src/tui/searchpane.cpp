/**
 * Copyright (C) 2025 rail5
 */

#include "widget.h"

Polonius::TUI::SearchPane::SearchPane(int x, int y, int w, int h)
	: Widget(x, y, w, h) {}

Polonius::TUI::SearchPane::SearchPane(Polonius::TUI::Edge anchor, int width, int height)
	: Widget(anchor, width, height) {}

void Polonius::TUI::SearchPane::setSearchInput(const std::string& input) {
	searchInput = input;
}

void Polonius::TUI::SearchPane::toggleEscapeSequences() {
	escapeSequencesEnabled = !escapeSequencesEnabled;
}

void Polonius::TUI::SearchPane::draw() {
	WINDOW* pane = getSubwindow();
	if (!pane) {
		return;
	}

	// Highlight the pane (invert colors)
	wattron(pane, A_REVERSE);
	// Fill with spaces
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			mvwaddch(pane, i, j, ' ');
		}
	}

	std::string label = "Search";
	if (regexEnabled) {
		label += " [Regex]";
	}

	if (caseSensitive) {
		label += " [Case Sensitive]";
	}

	if (escapeSequencesEnabled) {
		label += " [Esc]";
	}

	if (searchBackwards) {
		label += " [Backwards]";
	}

	label += ": ";
	searchLabel = label;

	// "Search: " label
	mvwprintw(pane, 0, 0, "%s", searchLabel.c_str());
	// Move cursor to the end of the label
	int label_length = static_cast<int>(searchLabel.length());
	wmove(pane, 0, label_length);

	// Print the search input
	mvwprintw(pane, 0, label_length, "%s", searchInput.c_str());

	cursorPosition.x = label_length + static_cast<int>(searchInput.length());
	cursorPosition.y = absoluteTop;

	wrefresh(pane);
	delwin(pane);
}

void Polonius::TUI::SearchPane::handleKeyPress(int ch) {
	switch (ch) {
		case KEY_LEFT:
			if (cursorPosition.x > static_cast<int>(searchLabel.length())) {
				cursorPosition.x--;
			}
			break;
		case KEY_RIGHT:
			if (cursorPosition.x < static_cast<int>(searchLabel.length() + searchInput.length())) {
				cursorPosition.x++;
			}
			break;
		case KEY_DOWN:
		case KEY_UP:
			// Do nothing for up/down keys in search pane
			break;
		case KEY_BACKSPACE:
			// Delete the character before the cursor
			if (cursorPosition.x > static_cast<int>(searchLabel.length())) {
				searchInput.erase(static_cast<size_t>(cursorPosition.x) - searchLabel.length() - 1, 1);
				cursorPosition.x--;
			}
			break;
		case KEY_DC: // Delete key
			// Delete the character at the cursor
			if (cursorPosition.x >= static_cast<int>(searchLabel.length())) {
				searchInput.erase(static_cast<size_t>(cursorPosition.x) - searchLabel.length(), 1);
			}
			break;
		case 18: // Ctrl+R
			regexEnabled = !regexEnabled;
			break;
		case 3: // Ctrl+C
			caseSensitive = !caseSensitive;
			break;
		case 5: // Ctrl+E
			escapeSequencesEnabled = !escapeSequencesEnabled;
			break;
		case 2: // Ctrl+B
			searchBackwards = !searchBackwards;
			break;
		case 10: // Enter key
			// TBD
			break;
		case 32 ... 126: // Printable characters
			// Write the character to the search input
			searchInput += static_cast<char>(ch);
			cursorPosition.x++;
			break;
		default:
			// Ignore
			break;
	}
	parent->refreshScreen();
}

const std::vector<Polonius::TUI::KeyShortcut> Polonius::TUI::SearchPane::widgetShortcuts() const {
	return {
		{"^C", "Case Sens"},
		{"^W", "Cancel"},
		{"^R", "Regex"},
		{"^B", "Backwards"},
		{"^E", "Esc.Seq."},
		{"^N", "Next"}
	};
}
