#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <memory>
#include <QWindow>

#include "core/size.h"

class QOpenGLContext;

namespace Boiler {
	class Renderer;
	class OpenGLRenderer;
}

class OpenGLWindow : public QWindow
{
	Q_OBJECT;

	std::unique_ptr<Boiler::OpenGLRenderer> renderer;
	QOpenGLContext *context;
	Boiler::Size currentSize;

	void processFrame();
	void exposeEvent(QExposeEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	bool event(QEvent *event);

public:
	OpenGLWindow();
	~OpenGLWindow();

signals:
	void rendererInitialized(Boiler::Renderer *renderer);
	void awaitingFrame();
};

#endif // OPENGLWINDOW_H
