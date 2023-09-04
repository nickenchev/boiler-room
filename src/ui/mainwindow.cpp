#include <QOpenGLContext>
#include <QKeyEvent>
#include <QFileDialog>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "openglwindow.h"
#include "assets/gltfimporter.h"
#include "display/renderer.h"
#include "models/assetitemmodel.h"

using namespace Boiler;

MainWindow::MainWindow(QWidget *parent) : QMainWindow{ parent }, logger("MainWindow")
{
	prevMouseX = 0;
	prevMouseY = 0;
	ui = new Ui_MainWindow();
	ui->setupUi(this);

	OpenGLWindow *openGLWindow = new OpenGLWindow(frameInfo);

    QWidget *openGLWidget = QWidget::createWindowContainer(openGLWindow);
    connect(openGLWindow, &OpenGLWindow::rendererInitialized, this, &MainWindow::onRendererInitialized);
	connect(openGLWindow, &OpenGLWindow::awaitingFrame, this, &MainWindow::onAwaitingFrame);

    ui->tabRender->layout()->addWidget(openGLWidget);

	// assets management
	QAbstractItemModel *assetItemModel = new AssetItemModel(this);
	ui->assetsTreeView->setModel(assetItemModel);
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

void MainWindow::onLoadModel()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load a Model"), QString(), tr("GLTF Asset (*.gltf)"));

	GLTFImporter importer(*engine);
	auto gltfModel = importer.import(engine->getRenderer().getAssetSet(), filename.toStdString());

	emit modelLoaded(gltfModel);
}
