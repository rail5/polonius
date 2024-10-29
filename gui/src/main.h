//
// Created by NAT on 10/27/2024.
//

// main.h

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

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

#endif // TEXTEDITOR_H