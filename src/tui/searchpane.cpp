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

	// "Search: " label
	mvwprintw(pane, 0, 0, "%s", searchLabel.c_str());
	// Move cursor to the end of the label
	int label_length = static_cast<int>(searchLabel.length());
	wmove(pane, 0, label_length);

	// Calculate the cursor position
	cursorPosition.x = label_length;
	cursorPosition.y = absoluteTop;

	wrefresh(pane);
	delwin(pane);
}

void Polonius::TUI::SearchPane::handleKeyPress(int ch) {
	switch (ch) {
		default:
			break;
	}
}

const std::vector<Polonius::TUI::KeyShortcut> Polonius::TUI::SearchPane::widgetShortcuts() const {
	return {
		{"^C", "Case Sens"},
		{"^W", "Cancel"},
		{"^R", "Regex"},
		{"^B", "Backwards"},
		{"^E", "Esc.Seq."}
	};
}
