#pragma once

#include <vector>
#include <QAbstractTableModel>
#include <QTimer>
#include "core/logentry.h"

class LogTableModel : public QAbstractTableModel
{
    Q_OBJECT;
    std::vector<Boiler::LogEntry> buffer;
    std::vector<Boiler::LogEntry> entries;
    QTimer timer;

    void logTimerCallback();

public:
    explicit LogTableModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void onEntryReceived(const Boiler::LogEntry &entry);

signals:
    void logUpdateFinished();
};
