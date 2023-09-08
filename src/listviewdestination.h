#pragma once

#include <QObject>
#include "core/logdestination.h"

class EntryForwarder : public QObject
{
    Q_OBJECT;
    
public:
    void emitEntry(const Boiler::LogEntry &entry);

signals:
    void entryAdded(const Boiler::LogEntry &entry);
};

class ListViewDestination : public Boiler::LogDestination
{
    EntryForwarder forwarder;

public:
    ListViewDestination();

    void log(const Boiler::LogEntry &entry);
    EntryForwarder &entryForwarder();
};
