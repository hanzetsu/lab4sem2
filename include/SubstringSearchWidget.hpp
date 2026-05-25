#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QThread>

class SubstringWorker : public QObject {
    Q_OBJECT
public:
    SubstringWorker(const QString& sourceText, const QString& pattern,
                    bool infinite, const QString& filePath = QString());
public slots:
    void run();
    void stop();
signals:
    void progress(int processed, int max);
    void resultReady(size_t count, bool truncated);
    void error(const QString& msg);
    void finished();
private:
    QString m_sourceText;
    QString m_pattern;
    bool m_infinite;
    QString m_filePath;
    bool m_cancelled;
};

class SubstringSearchWidget : public QWidget {
    Q_OBJECT
public:
     SubstringSearchWidget(QWidget *parent = nullptr);
    ~SubstringSearchWidget();

private slots:
    void onBrowseFile();
    void onSearch();
    void onCancel();
    void onResultReady(size_t count, bool truncated);
    void onError(const QString& msg);
    void onProgress(int processed, int max);
    void onWorkerFinished();

private:
    QRadioButton* m_stringRadio;
    QRadioButton* m_fileRadio;
    QRadioButton* m_lazyRadio;
    QLineEdit* m_stringEdit;
    QLineEdit* m_fileEdit;
    QPushButton* m_browseBtn;
    QLineEdit* m_patternEdit;
    QCheckBox* m_infiniteCheck;
    QPushButton* m_searchBtn;
    QPushButton* m_cancelBtn;
    QLabel* m_resultLabel;
    QProgressBar* m_progressBar;
    QString m_selectedFile;

    QThread* m_workerThread;
    SubstringWorker* m_worker;

    void setupUI();
    void disableUI(bool disable);
};