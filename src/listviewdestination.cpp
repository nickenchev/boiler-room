#include "listviewdestination.h"

ListViewDestination::ListViewDestination()
{
}

void ListViewDestination::log(const Boiler::LogEntry &entry)
{
	forwarder.emitEntry(entry);
}

EntryForwarder &ListViewDestination::entryForwarder()
{
    return forwarder;
}

void EntryForwarder::emitEntry(const Boiler::LogEntry &entry)
{
	emit entryAdded(entry);
}
