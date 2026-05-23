#include "StatisticsWidget.hpp"
#include "FileStream.hpp"
#include "ArrayStream.hpp"
#include "MutableArraySequence.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

StreamWorker::StreamWorker(std::shared_ptr<IStream<int>> stream, size_t windowSize)
    : m_stream(stream), m_running(true), m_windowSize(windowSize) {
    m_stats.windowSize = windowSize;
}

void StreamWorker::process() {
    const int BATCH_SIZE = 100;
    while (m_running && !m_stream->IsEnd() && !QThread::currentThread()->isInterruptionRequested()) {
        for (int i = 0; i < BATCH_SIZE && m_running && !m_stream->IsEnd() && !QThread::currentThread()->isInterruptionRequested(); ++i) {
            int val = m_stream->Read();
            m_stats.update(val);
        }
        emit statsUpdated(m_stats);
        QThread::msleep(10);
    }
    emit finished();
}

void StreamWorker::stop() {
    m_running = false;
}

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent), workerThread(nullptr), isRunning(false) {
    setupUI();
    qRegisterMetaType<StreamStatistics::OnlineStats>();
}

StatisticsWidget::~StatisticsWidget() {
    stopStream();
}

void StatisticsWidget::setupUI() {
    QVBoxLayout* main = new QVBoxLayout(this);
    QHBoxLayout* sourceLayout = new QHBoxLayout;
    sourceSelector = new QComboBox;
    sourceSelector->addItem("Файл (конечный)");
    sourceSelector->addItem("Массив 1..100 (конечный)");
    sourceSelector->addItem("Натуральные числа (бесконечный)");
    sourceSelector->addItem("Случайные числа 0..99 (бесконечный)");
    sourceLayout->addWidget(new QLabel("Источник:"));
    sourceLayout->addWidget(sourceSelector);

    QHBoxLayout* windowLayout = new QHBoxLayout;
    windowSizeSpin = new QSpinBox;
    windowSizeSpin->setRange(10, 10000);
    windowSizeSpin->setValue(100);
    windowLayout->addWidget(new QLabel("Размер окна медианы:"));
    windowLayout->addWidget(windowSizeSpin);

    startStopBtn = new QPushButton("Старт");
    countLabel = new QLabel("Обработано: 0");
    outputEdit = new QTextEdit;
    outputEdit->setReadOnly(true);

    main->addLayout(sourceLayout);
    main->addLayout(windowLayout);
    main->addWidget(startStopBtn);
    main->addWidget(countLabel);
    main->addWidget(outputEdit);

    connect(startStopBtn, &QPushButton::clicked, this, &StatisticsWidget::onStartStop);
}

void StatisticsWidget::onStartStop() {
    if (isRunning)
        stopStream();
    else
        startStream();
}

void StatisticsWidget::startStream() {
    int idx = sourceSelector->currentIndex();
    std::shared_ptr<IStream<int>> stream;
    try {
        if (idx == 0) {
            QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл с числами");
            if (fileName.isEmpty()) return;
            auto deser = [](const std::string& s) { return std::stoi(s); };
            stream = std::make_shared<FileStream<int>>(fileName.toStdString(), deser);
        } else if (idx == 1) {
            auto seq = std::make_shared<MutableArraySequence<int>>();
            for (int i = 1; i <= 100; ++i) seq->Append(i);
            stream = std::make_shared<ArrayStream<int>>(seq);
        } else if (idx == 2) {
            stream = std::make_shared<NaturalNumberStream>();
        } else {
            stream = std::make_shared<RandomNumberStream>();
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", e.what());
        return;
    }

    currentStream = stream;
    isRunning = true;
    startStopBtn->setText("Стоп");
    outputEdit->clear();

    workerThread = new QThread(this);
    StreamWorker* worker = new StreamWorker(stream, static_cast<size_t>(windowSizeSpin->value()));
    worker->moveToThread(workerThread);
    connect(workerThread, &QThread::started, worker, &StreamWorker::process);
    connect(worker, &StreamWorker::statsUpdated, this, &StatisticsWidget::updateDisplay);
    connect(worker, &StreamWorker::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

void StatisticsWidget::stopStream() {
    if (!workerThread || !isRunning) return;
    isRunning = false;
    workerThread->requestInterruption();
    StreamWorker* worker = workerThread->findChild<StreamWorker*>();
    if (worker) worker->stop();
    workerThread->quit();
    if (!workerThread->wait(3000)) {
        workerThread->terminate();
        workerThread->wait();
    }
    workerThread = nullptr;
    startStopBtn->setText("Старт");
}

void StatisticsWidget::updateDisplay(const StreamStatistics::OnlineStats& stats) {
    countLabel->setText(QString("Обработано: %1").arg(stats.count));
    StreamStatistics::OnlineStats tmp = stats;
    double median = tmp.getMedian();
    auto res = stats.getResult();
    outputEdit->clear();
    outputEdit->append(QString("Количество: %1").arg(res.count));
    outputEdit->append(QString("Сумма: %1").arg(res.sum));
    outputEdit->append(QString("Среднее: %1").arg(res.mean));
    outputEdit->append(QString("Дисперсия: %1").arg(res.variance));
    outputEdit->append(QString("Ср.кв.откл.: %1").arg(res.stddev));
    outputEdit->append(QString("Минимум: %1").arg(res.min));
    outputEdit->append(QString("Максимум: %1").arg(res.max));
    outputEdit->append(QString("Медиана (окно %1): %2").arg(windowSizeSpin->value()).arg(median));
}