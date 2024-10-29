/***
 * Copyright (C) 2024 Natnael Taddese
 * This is free software (GNU GPL v3)
 * Please refer to the LICENSE file for more information
*/

#include "class_TextEditor.cpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	TextEditor editor;
	editor.show();
	return app.exec();
}