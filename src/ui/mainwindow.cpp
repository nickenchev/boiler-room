#include <QOpenGLContext>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "openglwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow{ parent }
{
	ui = new Ui_MainWindow();
	ui->setupUi(this);

	OpenGLWindow *openGLWindow = new OpenGLWindow();
	QWidget *openGLWidget = this->createWindowContainer(openGLWindow);
	connect(openGLWindow, &OpenGLWindow::rendererInitialized, this, &MainWindow::onRendererInitialized);
	connect(openGLWindow, &OpenGLWindow::awaitingFrame, this, &MainWindow::onAwaitingFrame);

	openGLWidget->setParent(ui->centralwidget);
}

void MainWindow::onRendererInitialized(Boiler::Renderer *renderer)
{
	engine = std::make_unique<Boiler::Engine>();
	engine->initialize(renderer);

	editorPart = std::make_shared<Boiler::EditorPart>(*engine);
	engine->setPart(editorPart);
}

void MainWindow::onAwaitingFrame()
{
	Boiler::FrameInfo frameInfo;
	engine->step(frameInfo);
}
