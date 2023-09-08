#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <thread>
#include "core/engine.h"
#include "editorpart.h"
#include "../listviewdestination.h"

class Ui_MainWindow;
class QAbstractItemModel;

namespace Boiler
{
	class Renderer;
	class GLTFModel;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT;

    ListViewDestination logDestination;
	Boiler::Logger logger;
	std::unique_ptr<Boiler::Engine> engine;
	Ui_MainWindow *ui;
	std::shared_ptr<Boiler::EditorPart> editorPart;
	Boiler::FrameInfo frameInfo;
	int prevMouseX, prevMouseY;

	QAbstractItemModel *assetItemModel;
	std::thread loaderThread;

public:
	explicit MainWindow(QWidget *parent = nullptr);

private slots:
	void onLoaderComplete(std::shared_ptr<Boiler::GLTFModel> model);
    void onLogUpdateFinished();

public slots:
	void onRendererInitialized(Boiler::Renderer *renderer);
	void onAwaitingFrame();
	void onLoadModel();

signals:
	void loaderComplete(std::shared_ptr<Boiler::GLTFModel> model);
	void modelLoaded(std::shared_ptr<Boiler::GLTFModel> model);

};

#endif // MAINWINDOW_H
