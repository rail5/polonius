/***
 * Copyright (C) 2024 Natnael Taddese
*/

#ifndef GUI_SRC_CLASS_TEXTEDITOR_H_
#define GUI_SRC_CLASS_TEXTEDITOR_H_

#include <QMainWindow>
#include <QString>
#include <QTextEdit>

class TextEditor : public QMainWindow
{

public:
	TextEditor();
	~TextEditor();

private slots:
	void newFile();
	void openFile();
	bool saveFile();
	bool saveFileAs();
	void about();

private:
	void createActions();
	bool maybeSave();
	void setCurrentFile(const QString &fileName);

	QTextEdit *textEdit;
	QString currentFile;
	bool isUntitled;
};

#endif // GUI_SRC_CLASS_TEXTEDITOR_H_