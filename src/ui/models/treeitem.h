#pragma once

#include <memory>
#include <vector>
#include <QModelIndex>

class TreeItem
{
	std::vector<std::shared_ptr<TreeItem>> children;
	QModelIndex parent;

public:
	TreeItem(QModelIndex parent)
	{
		this->parent = parent;
	}

	QModelIndex getParent() { return parent;  }
	void addChild(std::shared_ptr<TreeItem> child)
	{
		children.push_back(child);
	}

	std::shared_ptr<TreeItem> getChild(unsigned int index)
	{
		return children[index];
	}

	virtual unsigned int childCount() const
	{
		return children.size();
	}

	virtual const std::string &getTitle() const = 0;
};

