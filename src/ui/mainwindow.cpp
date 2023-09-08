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
#include "models/loglistmodel.h"

using namespace Boiler;

MainWindow::MainWindow(QWidget *parent) : QMainWindow{ parent }, logger("MainWindow")
{
	prevMouseX = 0;
	prevMouseY = 0;
	ui = new Ui_MainWindow();
	ui->setupUi(this);

    Logger::setDestination(&logDestination);
	OpenGLWindow *openGLWindow = new OpenGLWindow(frameInfo);

    QWidget *openGLWidget = QWidget::createWindowContainer(openGLWindow);
    ui->renderLayout->addWidget(openGLWidget);

	connect(openGLWindow, &OpenGLWindow::rendererInitialized, this, &MainWindow::onRendererInitialized);
	connect(openGLWindow, &OpenGLWindow::awaitingFrame, this, &MainWindow::onAwaitingFrame);
	connect(this, &MainWindow::loaderComplete, this, &MainWindow::onLoaderComplete);

	// assets management
	QAbstractItemModel *assetItemModel = new AssetItemModel(this);
	ui->assetsTreeView->setModel(assetItemModel);

    // log model
    LogListModel *logListModel = new LogListModel(this);
    ui->logListView->setModel(logListModel);

    connect(&logDestination.entryForwarder(), &EntryForwarder::entryAdded, logListModel, &LogListModel::onEntryReceived);
    connect(logListModel, &LogListModel::logUpdateFinished, this, &MainWindow::onLogUpdateFinished);
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
    ui->logListView->scrollToBottom();
}

void MainWindow::onLoadModel()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load a Model"), QString(), tr("GLTF Asset (*.gltf)"));

	if (!filename.isEmpty())
	{
		ui->actionLoad_Model->setEnabled(false);

        // spawn a worker thread to load the model
		loaderThread = std::thread([filename, this]() {
			GLTFImporter importer(*engine);
			auto gltfModel = importer.import(engine->getRenderer().getAssetSet(), filename.toStdString());

            // notify (MainWindow) when work is done
			emit loaderComplete(gltfModel);
		});
	}
}
