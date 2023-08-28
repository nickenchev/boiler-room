#include <memory>
#include <QResizeEvent>

#include "glad/glad.h"
#include <QOpenGLContext>
#include <QSurface>
#include "display/opengl/openglrenderer.h"

#include "openglwindow.h"

OpenGLWindow::OpenGLWindow() : QWindow()
{
	context = nullptr;
	this->setSurfaceType(SurfaceType::OpenGLSurface);
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
	Q_UNUSED(event);
	if (isExposed())
	{
		processFrame();
	}
}

void OpenGLWindow::processFrame()
{
	if (isExposed())
	{
		bool needsInitialize = false;

		if (!context)
		{
			context = new QOpenGLContext(this);
			context->setFormat(requestedFormat());
			context->create();
			needsInitialize = true;
		}

		context->makeCurrent(this);

		if (needsInitialize)
		{
			gladLoadGL();

			// initialize Boiler's OpenGL renderer
			renderer = std::make_unique<Boiler::OpenGLRenderer>();
			renderer->initialize(currentSize);

			emit rendererInitialized(renderer.get());
		}

		emit awaitingFrame();

		context->swapBuffers(this);
		requestUpdate();
	}
}

bool OpenGLWindow::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::UpdateRequest:
		{
			processFrame();
			return true;
		}
		default:
			return QWindow::event(event);
	}
}

void OpenGLWindow::resizeEvent(QResizeEvent *event)
{
	currentSize = Boiler::Size(event->size().width(), event->size().height());
}

OpenGLWindow::~OpenGLWindow()
{
	if (renderer)
	{
		renderer->shutdown();
	}
}
