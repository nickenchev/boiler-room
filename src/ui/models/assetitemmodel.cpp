#include <array>
#include "assetitemmodel.h"
#include "../mainwindow.h"

AssetItemModel::AssetItemModel(MainWindow *mainWindow)
{
	connect(mainWindow, &MainWindow::modelLoaded, this, &AssetItemModel::onModelAdded);

	categories.push_back(std::make_shared<AssetCategory>("Models"));
}

QModelIndex AssetItemModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
	{
		// top section nodes
		return createIndex(row, column, categories[row].get());
	}
	else
	{
		return createIndex(row, column, static_cast<TreeItem *>(parent.internalPointer())->getChild(row).get());
	}
}

QModelIndex AssetItemModel::parent(const QModelIndex &child) const
{
	return static_cast<TreeItem *>(child.internalPointer())->getParent();
}

int AssetItemModel::rowCount(const QModelIndex &parent) const
{
	int count = 0;
	if (!parent.isValid())
	{
		count = categories.size();
	}
	else
	{
		const TreeItem *treeItem = static_cast<TreeItem *>(parent.internalPointer());
		count = treeItem->childCount();
	}
	return count;
}

int AssetItemModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant AssetItemModel::data(const QModelIndex &index, int role) const
{
	QVariant result;
	if (role == Qt::DisplayRole)
	{
		if (!index.parent().isValid())
		{
			result = QString::fromStdString(categories[index.row()]->getTitle());
		}
		else
		{
			TreeItem *treeItem = static_cast<TreeItem *>(index.internalPointer());
			result = QString::fromStdString(treeItem->getTitle());
		}
	}
	return result;
}

bool AssetItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
	QAbstractItemModel::insertRows(row, count, parent);
	return true;
}

std::shared_ptr<TreeItem> AssetItemModel::getItem(QModelIndex index) const
{
	if (index.parent().isValid() && !index.parent().parent().isValid())
	{
		return categories[index.parent().row()]->getChild(index.row());
	}
	return nullptr;
}

void AssetItemModel::onModelAdded(std::shared_ptr<Boiler::GLTFModel> model)
{
	auto modelCategory = categories[0];

	QModelIndex parent = createIndex(0, 0, modelCategory.get());
	beginInsertRows(parent, modelCategory->childCount(), modelCategory->childCount());

	// add model to list
	std::shared_ptr<ModelItem> modelItem = std::make_shared<ModelItem>(model, parent);
	modelCategory->addChild(modelItem);
	insertRows(modelCategory->childCount(), 1, parent);

	QModelIndex parentModel = createIndex(modelCategory->childCount() - 1, 0, modelItem.get());
	endInsertRows();

	// add each mesh of the current model
	beginInsertRows(parentModel, modelItem->childCount(), modelItem->childCount() + model->getImportResult().meshes.size());
	for (int i = 0; i < model->getImportResult().meshes.size(); ++i)
	{
		const Boiler::Mesh &mesh = model->getImportResult().meshes[i];
		modelItem->addChild(std::make_shared<MeshItem>(mesh, parentModel));

		insertRows(modelItem->childCount(), 1, parentModel);
	}
	endInsertRows();
}
