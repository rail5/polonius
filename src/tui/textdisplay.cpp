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

void Polonius::TUI::TextDisplay::setBufferTextLines(const std::string& newBuffer) {
	lines.clear();
	lines = explode(newBuffer, '\n', false, 0, true);
	lines.pop_back(); // Remove the last empty line if it exists
	scrollOffset = 0; // Reset scroll offset when setting a new buffer
}

void Polonius::TUI::TextDisplay::refreshBuffer(uint64_t newBufferStart) {
	// Read enough lines from the file to fill the screen
	Polonius::Block newBuffer = parent->getFile()->readLines(newBufferStart, editorBottom - editorTop + 1);
	// TODO(@rail5): Handle extremely long lines. We don't want the program to crash on long lines
	//  Crashing on long lines is the kind of failing in other editors that we're trying to avoid
	//  Proposal: A readLines() function in Polonius::File that returns a non-contiguous Polonius::Block
	//     Which, when it detects too long a line, will stop reading until the next line
	//     And set a marker signifying that a line was truncated
	//     The format of Blocks will allow an obvious signifier in the non-contiguous jump in position
	bufferStart = newBuffer.start;
	bufferEnd = newBuffer.end();
	setBufferTextLines(newBuffer.contents);
}

void Polonius::TUI::TextDisplay::refreshBuffer_backwards(uint64_t newBufferEnd) {
	Polonius::Block newBuffer = parent->getFile()->readLines_backwards(newBufferEnd, editorBottom - editorTop + 1);
	bufferStart = newBuffer.start;
	bufferEnd = newBuffer.end();
	setBufferTextLines(newBuffer.contents);
}

/**
 * @brief Refresh the text buffer with new content from the file (upward)
 * 
 * The procedure is:
 * 		1. Find the most recent newline character before the start of the current buffer
 * 		2. Set the new buffer start to be just after that newline character
 * 			Thereby loading one full line from above into the buffer
 * 		3. Set the buffer. Start position: that one, size: the block size
 */
void Polonius::TUI::TextDisplay::refreshBuffer_upward() {
	if (bufferStart == 0) {
		return; // No more to load from the file
	}
	// Find the most recent newline before the start of the current buffer
	Polonius::Block previous_newline = parent->getFile()->search_backwards(bufferStart - 1, -1, "\n");
	// Mark that position as the start of the new buffer (loading one full line into the buffer)
	uint64_t newBufferStart = 0;
	if (previous_newline.initialized) {
		newBufferStart = previous_newline.start + 1; // +1 to skip the newline character
	}
	// If no match was found, the new start of the buffer is the start of the file
	refreshBuffer(newBufferStart);
}

/**
 * @brief Refresh the text buffer with new content from the file (downward)
 * 
 * The procedure is:
 * 	1. Calculate the new buffer's size
 * 		This will be the maximum of:
 * 			- The block size
 * 			- The current buffer size
 * 
 * 	2. Calculate the new buffer's start position
 * 		Start with the current buffer's start position
 * 		Iterate through all of the lines in the current buffer
 * 			that will not be displayed in the new buffer
 * 			and add up the bytes in each of those lines
 * 		Finally, set the new buffer's start position as:
 * 			The old start position + the sum of those bytes
 * 
 * 	3. Set the new buffer
 * 		Call Polonius::File::readFromFile(new start, new size, true)
 * 		Where 'true' signifies that readFromFile should not stop reading
 * 			until it hits a newline character
 */
void Polonius::TUI::TextDisplay::refreshBuffer_downward() {
	if (bufferEnd == parent->getFile()->getSize() - 1) {
		return; // No more to load from the file
	}
	uint64_t newBufferStart = bufferStart;
	// Which line is the first line being displayed on screen?
	// If the scroll offset is 0, we are displaying the first line of the buffer
	// If the scroll offset is greater than 0, we are displaying the line at scrollOffset
	for (size_t i = 0; i < static_cast<size_t>(scrollOffset) + 1 && i < lines.size(); i++) {
		newBufferStart += lines[i].length() + 1; // +1 for the newline character
	}
	refreshBuffer(newBufferStart);
}

void Polonius::TUI::TextDisplay::scrollUp() {
	if (scrollOffset > 0) {
		scrollOffset--;
	} else {
		refreshBuffer_upward();
	}
	parent->refreshScreen(); // Refresh the screen to reflect the scroll change
}

void Polonius::TUI::TextDisplay::scrollDown() {
	int numberOfLinesBeingDisplayed = editorBottom - editorTop + 1;
	if (scrollOffset < static_cast<int>(lines.size()) - numberOfLinesBeingDisplayed) {
		scrollOffset++;
	} else {
		// Load more from the file if we're at the end of the buffer
		refreshBuffer_downward();
	}
	parent->refreshScreen(); // Refresh the screen to reflect the scroll change
}

void Polonius::TUI::TextDisplay::pageUp() {
	if (bufferStart == 0) {
		return; // No more to load from the file
	}
	uint64_t newBufferEnd = bufferStart - 1;
	for (size_t i = 0; i < static_cast<size_t>(scrollOffset) && i < lines.size(); i++) {
		newBufferEnd += lines[i].length() + 1; // +1 for the newline character
	}
	refreshBuffer_backwards(newBufferEnd);
	parent->refreshScreen(); // Refresh the screen to reflect the scroll change
}

void Polonius::TUI::TextDisplay::pageDown() {
	// What's the last line currently being displayed on screen?
	int numberOfLinesBeingDisplayed = editorBottom - editorTop + 1;
	int lastLineBeingDisplayed = scrollOffset + numberOfLinesBeingDisplayed;

	uint64_t displayEnd = bufferEnd;
	for (size_t i = lines.size() - 1; i >= static_cast<size_t>(lastLineBeingDisplayed); i--) {
		displayEnd -= lines[i].length();
	}
	if (displayEnd == parent->getFile()->getSize() - 1) {
		return; // No more to load from the file
	}

	// Calculate the new start position of the buffer
	uint64_t newBufferStart = bufferEnd + 1;
	for (int i = static_cast<int>(lines.size()) - 1; i >= lastLineBeingDisplayed; i--) {
		newBufferStart -= lines[static_cast<size_t>(i)].length() + 1;
	}
	refreshBuffer(newBufferStart);
	parent->refreshScreen();
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
			int maxWidth = parent->getRight() - parent->getLeft();
			std::size_t widthSz = maxWidth > 0 ? static_cast<std::size_t>(maxWidth) : 0;
			mvwprintw(window, i, 0, "%s", line.substr(0, widthSz).c_str()); // Print the line to the window
			if (line.length() > widthSz) {
				// Add a trailing '>' if the line is truncated
				wattron(window, A_STANDOUT); // Highlight the '>' character
				mvwaddch(window, i, widthSz - 1, '>'); // Add '>' at the end of the line
				wattroff(window, A_STANDOUT); // Turn off the highlight
			}
		}
	}
}

void Polonius::TUI::TextDisplay::draw() {
	// If the window margins have changed, refresh the buffer
	if (editorTop != parent->getTop() || editorBottom != parent->getBottom() - 1 ||
		editorLeft != parent->getLeft() || editorRight != parent->getRight() - 1) {
		bufferInitialized = false; // Reset buffer initialization flag
	}

	// Store the editor window margins
	editorTop = parent->getTop();
	editorBottom = parent->getBottom() - 1; 
	editorLeft = parent->getLeft();
	editorRight = parent->getRight() - 1;

	if (!bufferInitialized) {
		refreshBuffer(bufferStart); // bufferStart is initialized to 0 by default
		bufferInitialized = true;
	}

	// Create a sub-window for the text display
	WINDOW* textWindow = getSubwindow();
	if (!textWindow) {
		return; // Ensure the window was created successfully
	}
	// Draw the text in the buffer to the window
	drawText(textWindow);
	// Refresh the window to show the changes
	wrefresh(textWindow);
	delwin(textWindow); // Delete the window after drawing
}
