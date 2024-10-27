//
// Created by NAT on 10/27/2024.
//

// main.cpp

#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QKeySequence>

#include "main.h"

TextEditor::TextEditor()
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    createActions(); // This now handles all menu creation

    setCurrentFile("");
    setWindowTitle("Simple Editor");
    resize(800, 600);
}

void TextEditor::createActions()
{
    // File menu actions
    QMenu *fileMenu = menuBar()->addMenu("&File");

    // New
    QAction *newAct = new QAction("&New", this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &TextEditor::newFile);
    fileMenu->addAction(newAct);

    // Open
    QAction *openAct = new QAction("&Open", this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &TextEditor::openFile);
    fileMenu->addAction(openAct);

    // Save
    QAction *saveAct = new QAction("&Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &TextEditor::saveFile);
    fileMenu->addAction(saveAct);

    // Save As
    QAction *saveAsAct = new QAction("Save &As...", this);
    connect(saveAsAct, &QAction::triggered, this, &TextEditor::saveFileAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    // Exit
    QAction *exitAct = new QAction("E&xit", this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAct);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    // About
    QAction *aboutAct = new QAction("&About", this);
    connect(aboutAct, &QAction::triggered, this, &TextEditor::about);
    helpMenu->addAction(aboutAct);
}

void TextEditor::newFile()
{
    if (maybeSave())
    {
        textEdit->clear();
        setCurrentFile("");
    }
}

void TextEditor::openFile()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
        {
            QFile file(fileName);
            if (file.open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream in(&file);
                textEdit->setPlainText(in.readAll());
                setCurrentFile(fileName);
            }
        }
    }
}

bool TextEditor::saveFile()
{
    if (currentFile.isEmpty())
    {
        return saveFileAs();
    }

    QFile file(currentFile);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        setCurrentFile(currentFile);
        return true;
    }
    return false;
}

bool TextEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        setCurrentFile(fileName);
        return true;
    }
    return false;
}

bool TextEditor::maybeSave()
{
    if (textEdit->document()->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "Simple Editor",
                                   "The document has been modified.\n"
                                   "Do you want to save your changes?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save)
            return saveFile();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void TextEditor::about()
{
    QMessageBox::about(this, "About Simple Editor",
                       "A simple text editor created with Qt");
}

void TextEditor::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    isUntitled = fileName.isEmpty();
    if (isUntitled)
    {
        setWindowTitle("untitled.txt - Simple Editor");
    }
    else
    {
        setWindowTitle(QString("%1 - Simple Editor").arg(currentFile));
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TextEditor editor;
    editor.show();
    return app.exec();
}