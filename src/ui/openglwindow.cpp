#include <QResizeEvent>
#include "glad/glad.h"
#include <QOpenGLContext>
#include <QSurface>
#include "display/opengl/openglrenderer.h"
#include "openglwindow.h"
#include "core/frameinfo.h"

using namespace Boiler;

inline KeyCode toBoilerKey(int qtKey);

OpenGLWindow::OpenGLWindow(FrameInfo &frameInfo) : QWindow(), frameInfo(frameInfo), logger("Render Viewport")
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
			renderer = std::make_unique<OpenGLRenderer>();
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

void OpenGLWindow::keyPressEvent(QKeyEvent *event)
{
	frameInfo.keyInputEvents.addEvent(KeyInputEvent(toBoilerKey(event->key()), ButtonState::DOWN));
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *event)
{
	frameInfo.keyInputEvents.addEvent(KeyInputEvent(toBoilerKey(event->key()), ButtonState::UP));
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *event)
{
	// TODO: Ensure we don't need the global x/y pos
	frameInfo.mouseXPos = event->position().x();
	frameInfo.mouseYPos = event->position().y();
}

void OpenGLWindow::resizeEvent(QResizeEvent *event)
{
	currentSize = Size(event->size().width(), event->size().height());
	if (renderer)
	{
		renderer->resize(currentSize);
	}
}

OpenGLWindow::~OpenGLWindow()
{
}

inline KeyCode toBoilerKey(int qtKey)
{
	switch (qtKey)
	{
		case Qt::Key_W:
			return KeyCode::w;
		case Qt::Key_A:
			return KeyCode::a;
		case Qt::Key_S:
			return KeyCode::s;
		case Qt::Key_D:
			return KeyCode::d;
	}
}

