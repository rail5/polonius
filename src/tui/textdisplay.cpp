/**
 * Copyright (C) 2025 rail5
 */

#include "widget.h"
#include "../shared/explode.h"

Polonius::TUI::TextDisplay::TextDisplay(int x, int y, int w, int h)
	: Widget(x, y, w, h) {}

Polonius::TUI::TextDisplay::TextDisplay(Polonius::TUI::Edge anchor, int width, int height)
	: Widget(anchor, width, height) {}

int Polonius::TUI::TextDisplay::getTopEdge() const {
	return editorTop;
}

int Polonius::TUI::TextDisplay::getBottomEdge() const {
	return editorBottom;
}

int Polonius::TUI::TextDisplay::getLeftEdge() const {
	return editorLeft;
}

int Polonius::TUI::TextDisplay::getRightEdge() const {
	return editorRight;
}

void Polonius::TUI::TextDisplay::setBuffer(const std::string& newBuffer) {
	lines.clear();
	lines = explode(newBuffer, '\n', false, 0, true);
	lines.pop_back(); // Remove the last empty line if it exists
	scrollOffset = 0; // Reset scroll offset when setting a new buffer
}

void Polonius::TUI::TextDisplay::refreshBuffer() {
	uint64_t newBufferStart = bufferStart;
	int64_t newBufferSize = static_cast<int64_t>(std::max(Polonius::block_size, bufferEnd - bufferStart + 1));
	// Which line is the first line being displayed on screen?
	// If the scroll offset is 0, we are displaying the first line of the buffer
	// If the scroll offset is greater than 0, we are displaying the line at scrollOffset
	for (size_t i = 0; i < static_cast<size_t>(scrollOffset) + 1 && i < lines.size(); i++) {
		newBufferStart += lines[i].length() + 1; // +1 for the newline character
	}
	Polonius::Block newBuffer = parent->getFile()->readFromFile(newBufferStart, newBufferSize, true);
	bufferStart = newBuffer.start;
	bufferEnd = newBuffer.end();
	setBuffer(newBuffer.contents);
	scrollOffset = 0;
}

void Polonius::TUI::TextDisplay::setInitialBuffer(const Polonius::Block& initialBuffer) {
	bufferStart = initialBuffer.start;
	bufferEnd = initialBuffer.end();
	setBuffer(initialBuffer.contents);
}

void Polonius::TUI::TextDisplay::scrollUp() {
	if (scrollOffset > 0) {
		scrollOffset--;
		parent->refreshScreen(); // Refresh the screen to reflect the scroll change
	}
}

void Polonius::TUI::TextDisplay::scrollDown() {
	int numberOfLinesBeingDisplayed = editorBottom - editorTop + 1;
	if (scrollOffset < static_cast<int>(lines.size()) - numberOfLinesBeingDisplayed) {
		scrollOffset++;
	} else {
		// Load more from the file if we're at the end of the buffer
		refreshBuffer();
	}
	parent->refreshScreen(); // Refresh the screen to reflect the scroll change
}

void Polonius::TUI::TextDisplay::clearBuffer() {
	lines.clear();
	scrollOffset = 0; // Reset scroll offset when clearing the buffer
}

void Polonius::TUI::TextDisplay::drawText(WINDOW* window) {
	if (lines.empty()) {
		return; // Nothing to display
	}
	// Write the contents of the buffer to the window
	wmove(window, 0, 0); // Move to the top-left corner of the window
	// Line by line until we hit the bottom of the window
	for (int i = 0; i < parent->getBottom() - parent->getTop() && i < static_cast<int>(lines.size()); i++) {
		if (i + scrollOffset < static_cast<int>(lines.size())) {
			// Convert index to size_t to avoid signed/unsigned mismatch
			std::size_t index = static_cast<std::size_t>(i + scrollOffset);
			std::string line = lines[index];
			// Truncate the line if it exceeds the width of the window
			{
				int maxWidth = parent->getRight() - parent->getLeft();
				std::size_t widthSz = maxWidth > 0 ? static_cast<std::size_t>(maxWidth) : 0;
				if (line.length() > widthSz) {
					line = line.substr(0, widthSz);
				}
			}
			mvwprintw(window, i, 0, "%s", line.c_str()); // Print the line to the window
		}
	}
}

void Polonius::TUI::TextDisplay::draw() {
	// Get the width and height
	int width, height;
	int x = x_;
	int y = y_;

	int available_width = parent->getRight() - parent->getLeft();
	int available_height = parent->getBottom() - parent->getTop();

	// Store the editor window margins
	editorTop = parent->getTop();
	editorBottom = parent->getBottom() - 1; 
	editorLeft = parent->getLeft();
	editorRight = parent->getRight() - 1;

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
			width = w_ <= 0 ? available_width : w_; // Use the specified width or default to COLS
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
			height = h_ <= 0 ? available_height : h_; // Use the specified height or default to LINES
			break;
	}

	if (position == Polonius::TUI::RELATIVE) {
		switch (anchor) {
			case Polonius::TUI::LEFT:
				x = 0; // Left edge
				y = (available_height - height) / 2; // Center vertically
				break;
			case Polonius::TUI::RIGHT:
				x = COLS - width; // Right edge
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
	} else {
		// If absolute positioning, use the specified x and y
		if (x < 0) x = 0; // Ensure x is not negative
		if (y < 0) y = 0; // Ensure y is not negative
		if (x + width > available_width) width = COLS - x; // Ensure width does not exceed screen width
		if (y + height > available_height) height = LINES - y; // Ensure height does not exceed screen height
	}
	// Create a sub-window for the text display
	WINDOW* textWindow = subwin(parent->getScreen(), height, width, y, x);
	if (!textWindow) {
		return; // Ensure the window was created successfully
	}
	// Draw the text in the buffer to the window
	drawText(textWindow);
	// Refresh the window to show the changes
	wrefresh(textWindow);
	delwin(textWindow); // Delete the window after drawing
}
