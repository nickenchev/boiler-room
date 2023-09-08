#pragma once

#include <vector>
#include <QAbstractListModel>
#include <QTimer>
#include "core/logentry.h"

class LogListModel : public QAbstractListModel
{
    Q_OBJECT;
    std::vector<Boiler::LogEntry> buffer;
    std::vector<Boiler::LogEntry> entries;
    QTimer timer;

    void logTimerCallback();

public:
    explicit LogListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void onEntryReceived(const Boiler::LogEntry &entry);

signals:
    void logUpdateFinished();
};
