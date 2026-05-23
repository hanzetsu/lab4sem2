#include "SubstringSearchWidget.hpp"
#include "StringStream.hpp"
#include "LazySequenceStream.hpp"
#include "SubstringCounter.hpp"
#include "MutableArraySequence.hpp"
#include "LazySequence.hpp"
#include "FileStream.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

SubstringWorker::SubstringWorker(const QString& sourceText, const QString& pattern,
                                 bool infinite, const QString& filePath)
    : m_sourceText(sourceText), m_pattern(pattern), m_infinite(infinite),
      m_filePath(filePath), m_cancelled(false) {}

void SubstringWorker::run() {
    try {
        size_t count = 0;
        bool truncated = false;

        if (m_filePath.isEmpty() && !m_sourceText.isEmpty()) {
            if (!m_infinite) {
                auto stream = std::make_shared<StringStream>(m_sourceText.toStdString());
                count = SubstringCounter::countInStream(stream, m_pattern.toStdString());
                emit progress(100, 100);
            } else {
                std::string base = m_sourceText.toStdString();
                if (base.empty()) {
                    emit error("Строка для бесконечного повторения не может быть пустой");
                    emit finished();
                    return;
                }
                struct CycleGen : Generator<char> {
                    std::string data;
                    size_t pos = 0;
                    CycleGen(const std::string& s) : data(s) {}
                    char GetNext() override {
                        char c = data[pos % data.size()];
                        ++pos;
                        return c;
                    }
                    bool HasNext() const override { return true; }
                    Cardinal GetPotentialSize() const override { return Cardinal::Omega(); }
                    Generator<char>* Clone() const override { return new CycleGen(*this); }
                };
                auto gen = std::make_shared<CycleGen>(base);
                auto infiniteSeq = LazySequence<char, MutableArraySequence>::FromGenerator(gen);
                auto stream = std::make_shared<LazySequenceStream<char>>(infiniteSeq);

                const size_t MAX_SYMBOLS = 500000;
                auto pi = SubstringCounter::buildPrefixFunction(m_pattern.toStdString());
                int j = 0;
                size_t processed = 0;
                count = 0;
                while (processed < MAX_SYMBOLS && !stream->IsEnd() && !m_cancelled) {
                    char c = stream->Read();
                    ++processed;
                    while (j > 0 && c != m_pattern.toStdString()[j])
                        j = pi[j - 1];
                    if (c == m_pattern.toStdString()[j]) {
                        ++j;
                        if (j == (int)m_pattern.size()) {
                            ++count;
                            j = pi[j - 1];
                        }
                    }
                    if (processed % 1000 == 0) {
                        emit progress(processed, MAX_SYMBOLS);
                        QThread::msleep(1);
                    }
                }
                if (processed >= MAX_SYMBOLS && !stream->IsEnd())
                    truncated = true;
                emit progress(MAX_SYMBOLS, MAX_SYMBOLS);
            }
        } else if (!m_filePath.isEmpty()) {
            QFile file(m_filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                emit error("Не удалось открыть файл");
                emit finished();
                return;
            }
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();
            auto stream = std::make_shared<StringStream>(content.toStdString());
            count = SubstringCounter::countInStream(stream, m_pattern.toStdString());
            emit progress(100, 100);
        }

        if (m_cancelled) {
            emit resultReady(0, false);
        } else {
            emit resultReady(count, truncated);
        }
    } catch (const std::exception& e) {
        emit error(e.what());
    }
    emit finished();
}

void SubstringWorker::stop() {
    m_cancelled = true;
}

SubstringSearchWidget::SubstringSearchWidget(QWidget *parent)
    : QWidget(parent), m_workerThread(nullptr), m_worker(nullptr) {
    setupUI();
}

SubstringSearchWidget::~SubstringSearchWidget() {
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void SubstringSearchWidget::setupUI() {
    QVBoxLayout* main = new QVBoxLayout(this);

    QGroupBox* sourceGroup = new QGroupBox("Источник данных");
    QVBoxLayout* sourceLayout = new QVBoxLayout;

    m_stringRadio = new QRadioButton("Строка (StringStream)");
    m_stringRadio->setChecked(true);
    m_fileRadio = new QRadioButton("Файл (FileStream)");
    m_lazyRadio = new QRadioButton("Ленивая последовательность (LazySequence)");

    QHBoxLayout* stringLayout = new QHBoxLayout;
    m_stringEdit = new QLineEdit;
    m_stringEdit->setPlaceholderText("Введите строку");
    stringLayout->addWidget(m_stringEdit);
    stringLayout->addWidget(m_stringRadio);

    QHBoxLayout* fileLayout = new QHBoxLayout;
    m_fileEdit = new QLineEdit;
    m_fileEdit->setReadOnly(true);
    m_fileEdit->setPlaceholderText("Не выбран");
    m_browseBtn = new QPushButton("Обзор...");
    fileLayout->addWidget(m_fileEdit);
    fileLayout->addWidget(m_browseBtn);
    fileLayout->addWidget(m_fileRadio);

    QHBoxLayout* lazyLayout = new QHBoxLayout;
    m_infiniteCheck = new QCheckBox("Бесконечное повторение строки (осторожно!)");
    lazyLayout->addWidget(m_infiniteCheck);
    lazyLayout->addWidget(m_lazyRadio);

    sourceLayout->addLayout(stringLayout);
    sourceLayout->addLayout(fileLayout);
    sourceLayout->addLayout(lazyLayout);
    sourceGroup->setLayout(sourceLayout);

    QHBoxLayout* patternLayout = new QHBoxLayout;
    patternLayout->addWidget(new QLabel("Подстрока:"));
    m_patternEdit = new QLineEdit;
    patternLayout->addWidget(m_patternEdit);
    main->addLayout(patternLayout);

    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    m_searchBtn = new QPushButton("Подсчитать вхождения");
    m_cancelBtn = new QPushButton("Отмена");
    m_cancelBtn->setEnabled(false);
    m_resultLabel = new QLabel("Результат: ");

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_searchBtn);
    buttonLayout->addWidget(m_cancelBtn);

    main->addWidget(sourceGroup);
    main->addWidget(m_progressBar);
    main->addLayout(buttonLayout);
    main->addWidget(m_resultLabel);

    connect(m_browseBtn, &QPushButton::clicked, this, &SubstringSearchWidget::onBrowseFile);
    connect(m_searchBtn, &QPushButton::clicked, this, &SubstringSearchWidget::onSearch);
    connect(m_cancelBtn, &QPushButton::clicked, this, &SubstringSearchWidget::onCancel);
}

void SubstringSearchWidget::onBrowseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите текстовый файл");
    if (!fileName.isEmpty()) {
        m_selectedFile = fileName;
        m_fileEdit->setText(fileName);
        m_fileRadio->setChecked(true);
    }
}

void SubstringSearchWidget::onSearch() {
    QString pattern = m_patternEdit->text();
    if (pattern.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Подстрока не может быть пустой");
        return;
    }

    QString sourceText;
    QString filePath;
    bool infinite = false;

    if (m_stringRadio->isChecked()) {
        sourceText = m_stringEdit->text();
        if (sourceText.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите строку");
            return;
        }
    } else if (m_fileRadio->isChecked()) {
        if (m_selectedFile.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Выберите файл");
            return;
        }
        filePath = m_selectedFile;
    } else { // LazySequence
        sourceText = m_stringEdit->text();
        if (sourceText.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Для ленивой последовательности нужна строка");
            return;
        }
        infinite = m_infiniteCheck->isChecked();
    }

    disableUI(true);
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);

    m_workerThread = new QThread(this);
    m_worker = new SubstringWorker(sourceText, pattern, infinite, filePath);
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_worker, &SubstringWorker::run);
    connect(m_worker, &SubstringWorker::resultReady, this, &SubstringSearchWidget::onResultReady);
    connect(m_worker, &SubstringWorker::error, this, &SubstringSearchWidget::onError);
    connect(m_worker, &SubstringWorker::progress, this, &SubstringSearchWidget::onProgress);
    connect(m_worker, &SubstringWorker::finished, this, &SubstringSearchWidget::onWorkerFinished);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);

    m_workerThread->start();
}

void SubstringSearchWidget::onCancel() {
    if (m_worker) {
        m_worker->stop();
    }
    m_cancelBtn->setEnabled(false);
}

void SubstringSearchWidget::onResultReady(size_t count, bool truncated) {
    if (truncated)
        m_resultLabel->setText(QString("Результат (ограничено 500000 символов): %1 вхождений").arg(count));
    else
        m_resultLabel->setText(QString("Результат: %1 вхождений").arg(count));
}

void SubstringSearchWidget::onError(const QString& msg) {
    QMessageBox::critical(this, "Ошибка", msg);
    m_resultLabel->setText("Результат: ошибка");
}

void SubstringSearchWidget::onProgress(int processed, int max) {
    m_progressBar->setMaximum(max);
    m_progressBar->setValue(processed);
}

void SubstringSearchWidget::onWorkerFinished() {
    disableUI(false);
    m_progressBar->setVisible(false);
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
        m_workerThread = nullptr;
    }
    m_worker = nullptr;
}

void SubstringSearchWidget::disableUI(bool disable) {
    m_searchBtn->setEnabled(!disable);
    m_cancelBtn->setEnabled(disable);
    m_stringRadio->setEnabled(!disable);
    m_fileRadio->setEnabled(!disable);
    m_lazyRadio->setEnabled(!disable);
    m_browseBtn->setEnabled(!disable);
    m_stringEdit->setEnabled(!disable);
    m_patternEdit->setEnabled(!disable);
    m_infiniteCheck->setEnabled(!disable);
}