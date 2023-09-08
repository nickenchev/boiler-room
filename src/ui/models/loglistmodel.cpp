#include "loglistmodel.h"

LogListModel::LogListModel(QObject *parent) : QAbstractListModel{parent}
{
    buffer.reserve(100);
    entries.reserve(1000);
    connect(&timer, &QTimer::timeout, this, &LogListModel::logTimerCallback);
    timer.start(1000);
}

int LogListModel::rowCount(const QModelIndex &parent) const
{
    return entries.size();
}

QVariant LogListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (role == Qt::DisplayRole)
    {
        result = QString::fromStdString(entries[index.row()].getMessage());
    }
    return result;
}

void LogListModel::onEntryReceived(const Boiler::LogEntry &entry)
{
    buffer.push_back(entry);
}

void LogListModel::logTimerCallback()
{
    beginInsertRows(QModelIndex(), entries.size(), entries.size() + buffer.size());

    entries.insert(entries.end(), buffer.begin(), buffer.end());
    buffer.clear();
    insertRows(entries.size() - 1, buffer.size());

    endInsertRows();

    emit logUpdateFinished();
}
