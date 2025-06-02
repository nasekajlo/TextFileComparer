// Author: Sofiya Nasiakaila

#include "Mainwindow.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      file_path_edit1_(nullptr), file_path_edit2_(nullptr),
      open_button1_(nullptr), open_button2_(nullptr),
      compare_button_(nullptr), save_button_(nullptr),
      diff_table_(nullptr) {
    // Set up the UI components
    QWidget *central = new QWidget(this);
    QGridLayout *grid = new QGridLayout();

    // File 1 selection row
    QLabel *label1 = new QLabel(tr("Plik 1:"), this);
    file_path_edit1_ = new QLineEdit(this);
    file_path_edit1_->setReadOnly(true);
    open_button1_ = new QPushButton(tr("Wybierz..."), this);
    grid->addWidget(label1, 0, 0);
    grid->addWidget(file_path_edit1_, 0, 1);
    grid->addWidget(open_button1_, 0, 2);

    // File 2 selection row
    QLabel *label2 = new QLabel(tr("Plik 2:"), this);
    file_path_edit2_ = new QLineEdit(this);
    file_path_edit2_->setReadOnly(true);
    open_button2_ = new QPushButton(tr("Wybierz..."), this);
    grid->addWidget(label2, 1, 0);
    grid->addWidget(file_path_edit2_, 1, 1);
    grid->addWidget(open_button2_, 1, 2);

    // Compare and Save buttons on a horizontal layout
    compare_button_ = new QPushButton(tr("Porównaj"), this);
    save_button_ = new QPushButton(tr("Zapisz wynik"), this);
    save_button_->setEnabled(false); // disabled until a comparison is done
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addStretch();
    hbox->addWidget(compare_button_);
    hbox->addWidget(save_button_);
    grid->addLayout(hbox, 2, 0, 1, 3);  // span across all columns

    // Differences table
    diff_table_ = new QTableWidget(this);
    diff_table_->setColumnCount(3);
    diff_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    diff_table_->setWordWrap(true);
    diff_table_->verticalHeader()->setVisible(false);
    diff_table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    diff_table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    diff_table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    // Initially, no data in table
    grid->addWidget(diff_table_, 3, 0, 1, 3);

    central->setLayout(grid);
    setCentralWidget(central);
    setWindowTitle(tr("Porównywarka tekstu"));  // "Text Comparator"

    // Connect signals to slots
    connect(open_button1_, &QPushButton::clicked, this, &MainWindow::openFile1);
    connect(open_button2_, &QPushButton::clicked, this, &MainWindow::openFile2);
    connect(compare_button_, &QPushButton::clicked, this, &MainWindow::runComparison);
    connect(save_button_, &QPushButton::clicked, this, &MainWindow::saveResult);
}

MainWindow::~MainWindow() {
}

void MainWindow::openFile1() {
    QFileDialog dialog(this, tr("Wybierz plik tekstowy"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({ tr("Pliki tekstowe (*.txt *.md *.log *.csv)"), tr("Wszystkie pliki (*)") });
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setWindowModality(Qt::ApplicationModal);
    if (dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().first();
        if (!fileName.isEmpty()) {
            file_path_edit1_->setText(fileName);
        }
    }
}

void MainWindow::openFile2() {
    QFileDialog dialog(this, tr("Wybierz plik tekstowy"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({ tr("Pliki tekstowe (*.txt *.md *.log *.csv)"), tr("Wszystkie pliki (*)") });
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setWindowModality(Qt::ApplicationModal);
    if (dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().first();
        if (!fileName.isEmpty()) {
            file_path_edit2_->setText(fileName);
        }
    }
}

void MainWindow::runComparison() {
    QString path1 = file_path_edit1_->text();
    QString path2 = file_path_edit2_->text();
    if (path1.isEmpty() || path2.isEmpty()) {
        QMessageBox::warning(this, tr("Brak pliku"), tr("Proszę wybrać oba pliki przed porównaniem."));
        return;
    }
    // Parse files
    try {
        left_doc_ = FileParser::parseFile(path1.toStdString());
    } catch (boost::exception& ex) {
        // Extract file name info if available
        if (auto info = boost::get_error_info<boost::errinfo_file_name>(ex)) {
            QString msg = tr("Nie można otworzyć pliku: %1").arg(QString::fromStdString(*info));
            QMessageBox::critical(this, tr("Błąd"), msg);
        } else {
            QMessageBox::critical(this, tr("Błąd"), tr("Nie można odczytać pliku 1."));
        }
        return;
    }
    try {
        right_doc_ = FileParser::parseFile(path2.toStdString());
    } catch (boost::exception& ex) {
        if (auto info = boost::get_error_info<boost::errinfo_file_name>(ex)) {
            QString msg = tr("Nie można otworzyć pliku: %1").arg(QString::fromStdString(*info));
            QMessageBox::critical(this, tr("Błąd"), msg);
        } else {
            QMessageBox::critical(this, tr("Błąd"), tr("Nie można odczytać pliku 2."));
        }
        return;
    }
    // Compute differences
    std::vector<DiffOp> diff_ops = DiffEngine::compareDocuments(left_doc_, right_doc_);
    merger_ = std::make_unique<Merger>(left_doc_, right_doc_, diff_ops);
    // Populate the differences table in the UI
    populateDiffTable();
    save_button_->setEnabled(true);
}

void MainWindow::populateDiffTable() {
    const std::vector<DiffOp>& ops = merger_->getDiffs();
    diff_table_->clearContents();
    diff_table_->setRowCount(ops.size());
    // Set column headers to file names (or defaults if name not set)
    QString header1 = QFileInfo(file_path_edit1_->text()).fileName();
    QString header2 = QFileInfo(file_path_edit2_->text()).fileName();
    if (header1.isEmpty()) header1 = tr("Plik 1");
    if (header2.isEmpty()) header2 = tr("Plik 2");
    diff_table_->setHorizontalHeaderLabels({ header1, header2, tr("Akceptuj") });

    accept_buttons_.assign(ops.size(), nullptr);
    // Iterate diff operations and fill table rows
    for (size_t i = 0; i < ops.size(); ++i) {
        const DiffOp& op = ops[i];
        // Prepare left and right text for this diff op
        QString left_text, right_text;
        // We will use formatted HTML for highlighting differences
        if (op.type == DiffType::Equal) {
            // Unchanged paragraph: show in both columns, gray
            std::string utf8;
            left_doc_.paragraphs[op.left_index].text.toUTF8String(utf8);
            QString q = QString::fromUtf8(utf8.c_str());
            QString esc = q.toHtmlEscaped();
            left_text = "<span style=\"color: gray;\">" + esc + "</span>";
            right_text = "<span style=\"color: gray;\">" + esc + "</span>";
        } else if (op.type == DiffType::Remove && op.is_moved) {
            // Moved-out paragraph (exists in left, moved in right)
            std::string utf8;
            left_doc_.paragraphs[op.left_index].text.toUTF8String(utf8);
            QString esc = QString::fromUtf8(utf8.c_str()).toHtmlEscaped();
            // Highlight moved text in blue with strikethrough
            left_text = "<span style=\"color: blue; text-decoration: line-through;\">" + esc + "</span> <em>(przeniesiony)</em>";
            right_text = ""; // nothing on right side at this position
        } else if (op.type == DiffType::Add && op.is_moved) {
            // Moved-in paragraph (exists in right, was in left elsewhere)
            std::string utf8;
            right_doc_.paragraphs[op.right_index].text.toUTF8String(utf8);
            QString esc = QString::fromUtf8(utf8.c_str()).toHtmlEscaped();
            // Highlight moved text in blue with underline
            right_text = "<span style=\"color: blue; text-decoration: underline;\">" + esc + "</span> <em>(przeniesiony)</em>";
            left_text = ""; // nothing on left side here
        } else if (op.type == DiffType::Remove) {
            // Deleted paragraph (only in left)
            std::string utf8;
            left_doc_.paragraphs[op.left_index].text.toUTF8String(utf8);
            QString esc = QString::fromUtf8(utf8.c_str()).toHtmlEscaped();
            // Red strikethrough for text to remove
            left_text = "<span style=\"color: red; text-decoration: line-through;\">" + esc + "</span>";
            right_text = "";
        } else if (op.type == DiffType::Add) {
            // Added paragraph (only in right)
            std::string utf8;
            right_doc_.paragraphs[op.right_index].text.toUTF8String(utf8);
            QString esc = QString::fromUtf8(utf8.c_str()).toHtmlEscaped();
            // Green underline for added text
            right_text = "<span style=\"color: green; text-decoration: underline;\">" + esc + "</span>";
            left_text = "";
        } else if (op.type == DiffType::Modify) {
            // Modified paragraph: compute word-level diff and highlight changes
            const Paragraph& left_para = left_doc_.paragraphs[op.left_index];
            const Paragraph& right_para = right_doc_.paragraphs[op.right_index];
            // Compute LCS of words in this paragraph
            const auto& left_words = left_para.words;
            const auto& right_words = right_para.words;
            int n = left_words.size();
            int m = right_words.size();
            std::vector<int> dp((n+1)*(m+1));
            auto at = [&](int ii, int jj)->int& { return dp[ii*(m+1)+jj]; };
            for (int ii = 1; ii <= n; ++ii) {
                for (int jj = 1; jj <= m; ++jj) {
                    if (left_words[ii-1] == right_words[jj-1]) {
                        at(ii,jj) = at(ii-1,jj-1) + 1;
                    } else {
                        at(ii,jj) = (at(ii-1,jj) >= at(ii,jj-1)) ? at(ii-1,jj) : at(ii,jj-1);
                    }
                }
            }
            // Backtrack word LCS to identify diffs
            QString left_html, right_html;
            int ii = n, jj = m;
            while (ii > 0 || jj > 0) {
                if (ii > 0 && jj > 0 && left_words[ii-1] == right_words[jj-1]) {
                    // Common word
                    std::string word_utf8;
                    left_words[ii-1].toUTF8String(word_utf8);
                    QString esc = QString::fromUtf8(word_utf8.c_str()).toHtmlEscaped();
                    // Add common word to both outputs
                    left_html.prepend(esc);
                    right_html.prepend(esc);
                    // Prepend a space if not at start
                    if (ii != n || jj != m) {
                        left_html.prepend(" ");
                        right_html.prepend(" ");
                    }
                    --ii; --jj;
                } else if (jj > 0 && (ii == 0 || at(ii,jj-1) >= at(ii-1,jj))) {
                    // Word added in right
                    std::string word_utf8;
                    right_words[jj-1].toUTF8String(word_utf8);
                    QString esc = QString::fromUtf8(word_utf8.c_str()).toHtmlEscaped();
                    // Underline added word in green
                    right_html.prepend("<span style=\"color: green; text-decoration: underline;\">" + esc + "</span>");
                    // (no word on left side)
                    // Prepend space if not at start (for right side only, left gets no token)
                    if (jj != m) {
                        right_html.prepend(" ");
                    }
                    --jj;
                } else if (ii > 0 && (jj == 0 || at(ii,jj-1) < at(ii-1,jj))) {
                    // Word removed from left
                    std::string word_utf8;
                    left_words[ii-1].toUTF8String(word_utf8);
                    QString esc = QString::fromUtf8(word_utf8.c_str()).toHtmlEscaped();
                    // Strikethrough removed word in red
                    left_html.prepend("<span style=\"color: red; text-decoration: line-through;\">" + esc + "</span>");
                    // Prepend space if not at start (for left side only)
                    if (ii != n) {
                        left_html.prepend(" ");
                    }
                    --ii;
                } else {
                    // Should not happen
                    --ii; --jj;
                }
            }
            left_text = left_html;
            right_text = right_html;
        }

        // Create cell widgets to display formatted text
        QLabel *left_label = new QLabel();
        left_label->setTextFormat(Qt::RichText);
        left_label->setText(left_text);
        left_label->setWordWrap(true);
        left_label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        diff_table_->setCellWidget(i, 0, left_label);

        QLabel *right_label = new QLabel();
        right_label->setTextFormat(Qt::RichText);
        right_label->setText(right_text);
        right_label->setWordWrap(true);
        right_label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        diff_table_->setCellWidget(i, 1, right_label);

        if (op.type == DiffType::Equal) {
            // No accept button for equal (no change to accept)
            diff_table_->setCellWidget(i, 2, nullptr);
            accept_buttons_[i] = nullptr;
        } else {
            // Create Accept button for this change
            QPushButton *btn = new QPushButton(tr("Akceptuj"));
            btn->setStyleSheet("background-color: green; color: white; font-weight: bold;");
            diff_table_->setCellWidget(i, 2, btn);
            accept_buttons_[i] = btn;
            // Connect the button to accept this change
            connect(btn, &QPushButton::clicked, this, [this, i, btn]() {
                this->merger_->acceptChange(i);
                btn->setText(tr("✔"));
                btn->setStyleSheet("background-color: gray; color: white; font-weight: bold;");
                btn->setEnabled(false);
                // If it's a moved pair, disable the counterpart's button too
                const DiffOp& op = this->merger_->getDiffs()[i];
                if (op.is_moved) {
                    int j = op.matching_index;
                    if (j >= 0 && j < (int)this->accept_buttons_.size() && this->accept_buttons_[j]) {
                        this->accept_buttons_[j]->setText(tr("Zaakceptowano"));
                        this->accept_buttons_[j]->setStyleSheet("background-color: gray; color: white; font-weight: bold;");
                        this->accept_buttons_[j]->setEnabled(false);
                    }
                }
            });
        }
    }
    diff_table_->resizeRowsToContents();
}

void MainWindow::saveResult() {
    if (!merger_) return;

    QFileDialog dialog(this, tr("Zapisz plik wynikowy"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters({ tr("Pliki tekstowe (*.txt)"), tr("Wszystkie pliki (*)") });
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setWindowModality(Qt::ApplicationModal);
    if (dialog.exec() == QDialog::Accepted) {
        QString save_path = dialog.selectedFiles().first();
        if (!save_path.isEmpty()) {
            try {
                merger_->saveMerged(save_path.toStdString());
                QMessageBox::information(this, tr("Zapisano"), tr("Plik wynikowy został zapisany pomyślnie."));
            } catch (boost::exception& ex) {
                if (auto info = boost::get_error_info<boost::errinfo_file_name>(ex)) {
                    QString msg = tr("Nie udało się zapisać pliku: %1").arg(QString::fromStdString(*info));
                    QMessageBox::critical(this, tr("Błąd zapisu"), msg);
                } else {
                    QMessageBox::critical(this, tr("Błąd zapisu"), tr("Nie udało się zapisać pliku wynikowego."));
                }
            }
        }
    }
}