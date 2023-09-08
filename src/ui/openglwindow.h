#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <memory>
#include <QWindow>

#include "core/size.h"
#include "core/logger.h"

class QOpenGLContext;

namespace Boiler {
	class Renderer;
	class OpenGLRenderer;
	struct FrameInfo;
}

class OpenGLWindow : public QWindow
{
	Q_OBJECT;

	std::unique_ptr<Boiler::OpenGLRenderer> renderer;
	QOpenGLContext *context;
	Boiler::Size currentSize;
	Boiler::FrameInfo &frameInfo;
	Boiler::Logger logger;

	void processFrame();

	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

	void exposeEvent(QExposeEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	bool event(QEvent *event) override;

public:
	OpenGLWindow(Boiler::FrameInfo &frameInfo);
	~OpenGLWindow();

signals:
	void rendererInitialized(Boiler::Renderer *renderer);
	void awaitingFrame();
};

#endif // OPENGLWINDOW_H
