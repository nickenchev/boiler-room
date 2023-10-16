#include "logtablemodel.h"

LogTableModel::LogTableModel(QObject *parent) : QAbstractTableModel{parent}
{
    buffer.reserve(100);
    entries.reserve(1000);
    connect(&timer, &QTimer::timeout, this, &LogTableModel::logTimerCallback);
    timer.start(1000);
}

int LogTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

int LogTableModel::rowCount(const QModelIndex &parent) const
{
    return entries.size();
}

QVariant LogTableModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            result = QString::fromStdString(entries[index.row()].getSource());
        }
        else if (index.column() == 1)
        {
            result = QString::fromStdString(entries[index.row()].getMessage());
        }
    }
    return result;
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == 0)
        {
            result = "Source";
        }
        else if (section == 1)
        {
            result = "Message";
        }
    }
    return result;
}

void LogTableModel::onEntryReceived(const Boiler::LogEntry &entry)
{
    buffer.push_back(entry);
}

void LogTableModel::logTimerCallback()
{
    if (buffer.size())
    {
        beginInsertRows(QModelIndex(), entries.size(), entries.size() + buffer.size() - 1);

        entries.insert(entries.end(), buffer.begin(), buffer.end());
        buffer.clear();
        insertRows(entries.size() - 1, buffer.size());

        endInsertRows();

        emit logUpdateFinished();
    }
}
