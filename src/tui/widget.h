/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_TUI_WIDGET_H_
#define SRC_TUI_WIDGET_H_

#include <ncurses.h>
#include <string>

#include "window.h"

namespace Polonius {
namespace TUI {


enum Positioning {
	RELATIVE,
	ABSOLUTE
};

enum Edge {
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

enum RelativeSize : int {
	FULL = -1,
	HALF = -2,
	QUARTER = -3,
	THREE_QUARTERS = -4
};

/**
 * @class Widget
 * @brief Base class for all widgets in the Terminal UI
 * 
 * This class provides a base for all widgets in the Terminal UI.
 * It includes positioning, size, and a pure virtual draw function that must be implemented by derived classes.
 */
class Widget {
	protected:
		Polonius::TUI::Window* parent;
		int x_ = 0;
		int y_ = 0;
		int w_ = Polonius::TUI::FULL; // Default to full width
		int h_ = Polonius::TUI::FULL; // Default to full height
		int width = 0; // Actual width of the widget
		int height = 0; // Actual height of the widget
		Polonius::TUI::Positioning position = Polonius::TUI::RELATIVE;
		Polonius::TUI::Edge anchor = Polonius::TUI::BOTTOM;
		WINDOW* getSubwindow();
		Polonius::TUI::Cursor cursorPosition;

		int absoluteTop = 0;
		int absoluteLeft = 0;
	public:
		Widget();
		Widget(int x, int y, int w, int h);
		Widget(Polonius::TUI::Edge anchor, int width, int height);

		virtual ~Widget();

		virtual void draw();

		void setParent(Polonius::TUI::Window* parent);

		int getX() const;
		int getY() const;
		int getWidth() const;
		int getHeight() const;
		Polonius::TUI::Edge getAnchor() const;
		bool isRelativelyPositioned() const;

		int getAbsoluteTop() const;
		int getAbsoluteLeft() const;

		void setX(int x);
		void setY(int y);
		void setWidth(int w);
		void setHeight(int h);
		void setPositioning(Polonius::TUI::Positioning pos);

		void takeCursor();

		virtual const std::vector<Polonius::TUI::KeyShortcut> widgetShortcuts() const;

		virtual void handleKeyPress(int ch);
};

/**
 * @class TextDisplay
 * @brief The widget that controls the text editing functionality in the Terminal UI
 * 
 * Because Polonius never reads the entire file into memory,
 * this widget contains a buffer of size Polonius::block_size with text from the file
 * and only displays a *portion* of that buffer (as much as will fit on screen).
 * 
 * When the user scrolls the window, we change the part of the buffer we display
 * I.e, without updating the buffer, we change which part of it we're displaying to
 * reflect the user's scrolled position.
 * 
 * When the user nears the bottom or top of the buffer,
 * the buffer is refreshed with new content from the file, simulating an ordinary
 * text editor.
 */
class TextDisplay : public Widget {
	private:
		std::vector<std::string> lines; // The lines of text in the buffer
		int scrollOffset = 0; // The offset for scrolling through the buffer
		bool bufferInitialized = false;

		// Margins of the editor window:
		int editorTop = 0;
		int editorBottom = 0;
		int editorLeft = 0;
		int editorRight = 0;

		uint64_t bufferStart = 0;
		uint64_t bufferEnd = 0;

		void setBufferTextLines(const std::string& newBuffer); // Set the buffer to a new string
		void refreshBuffer_downward(); // Refresh the buffer with new content from the file
		void refreshBuffer_upward();
		void drawText(WINDOW* window); // Draw the text in the buffer to the window

	public:
		TextDisplay(int x, int y, int w, int h);
		TextDisplay(Polonius::TUI::Edge anchor, int width, int height);

		int getTopEdge() const;
		int getBottomEdge() const;
		int getLeftEdge() const;
		int getRightEdge() const;

		void draw() override;

		void refreshBuffer(uint64_t newBufferStart);
		void refreshBuffer_backwards(uint64_t newBufferEnd);

		void scrollUp(); // Scroll up in the buffer
		void scrollDown(); // Scroll down in the buffer
		void pageUp(); // Scroll up one page
		void pageDown(); // Scroll down one page
		void clearBuffer(); // Clear the buffer

		void handleKeyPress(int ch) override;

		const std::vector<Polonius::TUI::KeyShortcut> widgetShortcuts() const override;
};

/**
 * @class SearchPane
 * @brief A pane that allows searching through the text in the Terminal UI
 */
class SearchPane : public Widget {
	private:
		std::string searchInput;
		bool escapeSequencesEnabled = false;
		std::string searchLabel = "Search: ";
		bool initialized = false;

	public:
		SearchPane(int x, int y, int w, int h);
		SearchPane(Polonius::TUI::Edge anchor, int width, int height);

		void setSearchInput(const std::string& input);
		void toggleEscapeSequences();

		void draw() override;

		void handleKeyPress(int ch) override;

		const std::vector<Polonius::TUI::KeyShortcut> widgetShortcuts() const override;
};

/**
 * @class HelpPane
 * @brief A pane that displays help information in the Terminal UI
 * 
 * This class extends Widget to provide a pane that can display help information.
 * It includes a top label and a bottom label, which can be set and displayed.
 */
class HelpPane : public Widget {
	private:
		std::string topLabel;
		std::string bottomLabel;

		std::vector<Polonius::TUI::KeyShortcut> shortcuts;
	public:
		HelpPane(int x, int y, int w, int h, const std::string& label);
		HelpPane(Polonius::TUI::Edge anchor, int width, int height, const std::string& label);
		void setBottomLabel(const std::string& label);

		void draw() override;

		void setShortcuts(const std::vector<Polonius::TUI::KeyShortcut>& newShortcuts);
};

enum Color : uint8_t {
	DEFAULT = 0,
	RED = 10,
	GREEN = 11,
	YELLOW = 12
};

class Message : public Widget {
	private:
		std::string message;
		Polonius::TUI::Color color = Polonius::TUI::DEFAULT;
	public:
		Message();
		explicit Message(const std::string& msg);

		void draw() override;
		void setMessage(const std::string& msg);
		void setColor(Polonius::TUI::Color color);
};

} // namespace TUI
} // namespace Polonius

#endif // SRC_TUI_WIDGET_H_
