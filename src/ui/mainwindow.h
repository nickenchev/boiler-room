#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "core/engine.h"
#include "editorpart.h"

class Ui_MainWindow;

namespace Boiler
{
	class Renderer;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT;

	std::unique_ptr<Boiler::Engine> engine;
	Ui_MainWindow *ui;
	std::shared_ptr<Boiler::EditorPart> editorPart;

public:
	explicit MainWindow(QWidget *parent = nullptr);

public slots:
	void onRendererInitialized(Boiler::Renderer *renderer);
	void onAwaitingFrame();

};

#endif // MAINWINDOW_H
