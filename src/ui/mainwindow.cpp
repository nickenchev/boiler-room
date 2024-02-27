#include <QOpenGLContext>
#include <QKeyEvent>
#include <QFileDialog>
#include <thread>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "openglwindow.h"
#include "assets/gltfimporter.h"
#include "display/renderer.h"
#include "models/assetitemmodel.h"
#include "models/logtablemodel.h"

using namespace Boiler;

MainWindow::MainWindow(QWidget *parent) : QMainWindow{ parent }, logger("MainWindow")
{
	prevMouseX = 0;
	prevMouseY = 0;
	ui = new Ui_MainWindow();
	ui->setupUi(this);

	Logger::setDestination(&logDestination);
	openGLWindow = new OpenGLWindow(frameInfo);

	QWidget *openGLWidget = QWidget::createWindowContainer(openGLWindow);
	ui->renderLayout->addWidget(openGLWidget);

	connect(openGLWindow, &OpenGLWindow::rendererInitialized, this, &MainWindow::onRendererInitialized);
	connect(openGLWindow, &OpenGLWindow::awaitingFrame, this, &MainWindow::onAwaitingFrame);
	connect(this, &MainWindow::loaderComplete, this, &MainWindow::onLoaderComplete);

	// assets management
	QAbstractItemModel *assetItemModel = new AssetItemModel(this);
	ui->assetsTreeView->setModel(assetItemModel);

	connect(ui->assetsTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::selectionChanged);

	// log model
	LogTableModel *logTableModel = new LogTableModel(this);
	ui->logTable->setModel(logTableModel);

	connect(&logDestination.entryForwarder(), &EntryForwarder::entryAdded, logTableModel, &LogTableModel::onEntryReceived);
	connect(logTableModel, &LogTableModel::logUpdateFinished, this, &MainWindow::onLogUpdateFinished);
}

void MainWindow::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QModelIndexList indexList = selected.indexes();

	if (indexList.size() == 1)
	{
		AssetItemModel *model = static_cast<AssetItemModel *>(ui->assetsTreeView->model());
		auto item = model->getItem(indexList[0]);
		selectedAsset = item;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	engine->shutdown();
}

void MainWindow::onRendererInitialized(Renderer *renderer)
{
	engine = std::make_unique<Engine>();
	engine->initialize(renderer);

	editorPart = std::make_shared<EditorPart>(*engine);
	engine->setPart(editorPart);
}

void MainWindow::onAwaitingFrame()
{
	engine->step(frameInfo);
	frameInfo.keyInputEvents.reset();

	QSize labelSize = ui->label->size();
	QImage image = openGLWindow->grabQImage();

	ui->label->setPixmap(QPixmap::fromImage(image).scaled(labelSize.width(), labelSize.height(), Qt::AspectRatioMode::KeepAspectRatio));
}

void MainWindow::onLoaderComplete(std::shared_ptr<Boiler::GLTFModel> model)
{
	// wait for thread to complete if needed
	if (loaderThread.joinable())
	{
		loaderThread.join();
	}

	emit modelLoaded(model);
	ui->actionLoad_Model->setEnabled(true);
}

void MainWindow::onLogUpdateFinished()
{
	ui->logTable->scrollToBottom();
}

void MainWindow::onCreateInstance()
{
	if (selectedAsset)
	{
		auto modelItem = std::static_pointer_cast<ModelItem>(selectedAsset);
		auto gltfModel = modelItem->getModel();

		EntityComponentSystem &ecs = engine->getEcs();
		Entity newEntity = ecs.newEntity("New Entity");
		gltfModel->createInstance(ecs, newEntity);
	}
}

void MainWindow::onLoadModel()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load a Model"), QString(), tr("GLTF Asset (*.gltf)"));

	if (!filename.isEmpty())
	{
		ui->actionLoad_Model->setEnabled(false);

		GLTFImporter importer(*engine);
		auto gltfModel = importer.import(engine->getRenderer().getAssetSet(), filename.toStdString());
		// spawn a worker thread to load the model
		//loaderThread = std::thread([filename, this]() {
		//	GLTFImporter importer(*engine);
		//	auto gltfModel = importer.import(engine->getRenderer().getAssetSet(), filename.toStdString());

		//	// notify (MainWindow) when work is done
		//	emit loaderComplete(gltfModel);
		//	});
		emit loaderComplete(gltfModel);
	}
}

void MainWindow::onShutdown()
{
	engine->shutdown();
}
