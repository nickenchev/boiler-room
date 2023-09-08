#pragma once

#include <QAbstractItemModel>
#include <vector>
#include <memory>
#include "assets/gltfmodel.h"
#include "treeitem.h"
#include "display/mesh.h"

namespace Boiler
{
	class GLTFModel;
}

class MainWindow;

class AssetCategory : public TreeItem
{
	std::string name;

public:
	AssetCategory(const std::string name) : TreeItem(QModelIndex())
	{
		this->name = name;
	}

	const std::string &getTitle() const override { return name; }
};

class MeshItem : public TreeItem
{
	const Boiler::Mesh *mesh;

public:
	MeshItem(const Boiler::Mesh &mesh, QModelIndex parent) : TreeItem(parent)
	{
		this->mesh = &mesh;
	}

	const Boiler::Mesh *getMesh() const { return mesh; }
	const std::string &getTitle() const override { return mesh->name; }
};

class ModelItem : public TreeItem
{
	std::shared_ptr<Boiler::GLTFModel> model;

public:
	ModelItem(std::shared_ptr<Boiler::GLTFModel> model, QModelIndex parent) : TreeItem(parent)
	{
		this->model = model;
	}
	const std::string &getTitle() const override { return model->getFilePath(); }
};

class AssetItemModel : public QAbstractItemModel
{
	Q_OBJECT;

	std::vector<std::shared_ptr<AssetCategory>> categories;

public:
	AssetItemModel(MainWindow *mainWindow);

	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public slots:
	void onModelAdded(std::shared_ptr<Boiler::GLTFModel> model);
};
