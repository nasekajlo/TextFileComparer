// Author: Sofiya Nasiakaila

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include "Document.h"
#include "FileParser.h"
#include "Diff.h"
#include "Merger.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile1();
    void openFile2();
    void runComparison();
    void saveResult();

private:
    void populateDiffTable();

    // UI elements
    QLineEdit *file_path_edit1_;
    QLineEdit *file_path_edit2_;
    QPushButton *open_button1_;
    QPushButton *open_button2_;
    QPushButton *compare_button_;
    QPushButton *save_button_;
    QTableWidget *diff_table_;

    // Data and logic
    Document left_doc_;
    Document right_doc_;
    std::unique_ptr<Merger> merger_;
    std::vector<QPushButton*> accept_buttons_;  // One per diff row (nullptr for equal rows)
};

#endif // MAINWINDOW_H
