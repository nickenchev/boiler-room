#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "core/engine.h"
#include "editorpart.h"

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

	Boiler::Logger logger;
	std::unique_ptr<Boiler::Engine> engine;
	Ui_MainWindow *ui;
	std::shared_ptr<Boiler::EditorPart> editorPart;
	Boiler::FrameInfo frameInfo;
	int prevMouseX, prevMouseY;

	QAbstractItemModel *assetItemModel;

public:
	explicit MainWindow(QWidget *parent = nullptr);

public slots:
	void onRendererInitialized(Boiler::Renderer *renderer);
	void onAwaitingFrame();
	void onLoadModel();

signals:
	void modelLoaded(std::shared_ptr<Boiler::GLTFModel> model);

};

#endif // MAINWINDOW_H
