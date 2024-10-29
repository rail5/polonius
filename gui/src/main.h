//
// Created by NAT on 10/27/2024.
//

// main.h

#ifndef GUI_SRC_MAIN_H_
#define GUI_SRC_MAIN_H_

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

#endif // GUI_SRC_MAIN_H_