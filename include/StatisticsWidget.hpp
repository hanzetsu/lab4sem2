#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <memory>
#include <QThread>
#include "IStream.hpp"
#include "StreamStatistics.hpp"

class StreamWorker : public QObject {
    Q_OBJECT
public:
    StreamWorker(std::shared_ptr<IStream<int>> stream, size_t windowSize);
public slots:
    void process();
    void stop();
signals:
    void statsUpdated(const StreamStatistics::OnlineStats& stats);
    void finished();
private:
    std::shared_ptr<IStream<int>> m_stream;
    StreamStatistics::OnlineStats m_stats;
    bool m_running;
    size_t m_windowSize;
};

class StatisticsWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();
private slots:
    void onStartStop();
    void updateDisplay(const StreamStatistics::OnlineStats& stats);
private:
    QComboBox* sourceSelector;
    QPushButton* startStopBtn;
    QTextEdit* outputEdit;
    QSpinBox* windowSizeSpin;
    QLabel* countLabel;
    std::shared_ptr<IStream<int>> currentStream;
    QThread* workerThread;
    bool isRunning;
    void setupUI();
    void startStream();
    void stopStream();
};