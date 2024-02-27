#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <memory>
#include <thread>
#include "core/engine.h"
#include "editorpart.h"
#include "../listviewdestination.h"

class Ui_MainWindow;
class QAbstractItemModel;
class OpenGLWindow;

namespace Boiler
{
	class Renderer;
	class GLTFModel;
}

class TreeItem;

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

	std::thread loaderThread;
	std::shared_ptr<TreeItem> selectedAsset;

	OpenGLWindow *openGLWindow;

	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:
	void closeEvent(QCloseEvent *event) override;

public:
	explicit MainWindow(QWidget *parent = nullptr);

private slots:
	void onLoaderComplete(std::shared_ptr<Boiler::GLTFModel> model);
    void onLogUpdateFinished();

public slots:
	void onRendererInitialized(Boiler::Renderer *renderer);
	void onAwaitingFrame();
	void onLoadModel();
	void onShutdown();
    void onCreateInstance();

signals:
	void loaderComplete(std::shared_ptr<Boiler::GLTFModel> model);
	void modelLoaded(std::shared_ptr<Boiler::GLTFModel> model);

};

#endif // MAINWINDOW_H
