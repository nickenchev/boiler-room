#include <SDL_mouse.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <boiler.h>
#include <display/vulkan/vulkanrenderer.h>
#include <display/opengl/openglrenderer.h>
#include <input/keys.h>
#include <glad/glad.h>
#include "editorpart.h"

using namespace Boiler;

LogDestination logDestination;

class OpenGLSDLWindow
{
	SDL_Window *win = nullptr;
	Size initialSize;
	Logger logger;

public:
	OpenGLSDLWindow(Size &initialSize) : logger("Window")
	{
		// create an SDL window
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
		{
			// Request an OpenGL 4.5 context (should be core)
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			// Also request a depth buffer
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

			SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

			win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				initialSize.width, initialSize.height, winFlags);
			if (!win)
			{
				std::cout << "Window Error: " << SDL_GetError() << std::endl;
				throw std::runtime_error("Unable to create main window");
			}

			SDL_GLContext glContext = SDL_GL_CreateContext(win);
			SDL_GL_SetSwapInterval(1);
			SDL_SetRelativeMouseMode(SDL_TRUE);

			if (!gladLoadGL())
			{
				throw std::runtime_error("Error loading OpenGL proc addresses");
			}
		}
		else
		{
			throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
		}
	}

	~OpenGLSDLWindow()
	{
		SDL_DestroyWindow(win);
	}

	void endFrame()
	{
		SDL_GL_SwapWindow(win);
	}

	void processEvents(Engine &engine, FrameInfo &frameInfo)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				{
					engine.quit();
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_RESIZED:
						{
							const Size newSize(static_cast<float>(event.window.data1), static_cast<float>(event.window.data2));
							engine.getRenderer().resize(newSize);
							break;
						}
						case SDL_WINDOWEVENT_CLOSE:
						{
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED:
						{
							break;
						}
						case SDL_WINDOWEVENT_SHOWN:
						{
							break;
						}
					}
					break;
				}
				case SDL_FINGERUP:
				{
					/*
					TouchTapEvent event(TapState::UP);
					for (const TouchTapEventListener &listener : touchTapEventListeners)
					{
						listener(event);
					}
					*/
					break;
				}
				case SDL_FINGERDOWN:
				{
					/*
					TouchTapEvent event(TapState::DOWN);
					for (const TouchTapEventListener &listener : touchTapEventListeners)
					{
						listener(event);
					}
					*/
					break;
				}
				case SDL_FINGERMOTION:
				{
					/*
					TouchMotionEvent motionEvent(event.tfinger.x, event.tfinger.y,
												 event.tfinger.dx, event.tfinger.dy);
					for (const TouchMotionListener &listener : touchMotionListeners)
					{
						listener(motionEvent);
					}
					*/
					break;
				}
				//case SDL_MOUSEWHEEL:
				//{
				//	static_cast<GUISystem *>(guiSystem)->mouseWheel(event.wheel.preciseX, event.wheel.preciseY);
				//	break;
				//}
				case SDL_MOUSEMOTION:
				{
					//if (!mouseRelativeMode)
					//{
					//	static_cast<GUISystem *>(guiSystem)->mouseMove(event.motion.x, event.motion.y);
					//}
					//else
					{
						frameInfo.mouseXPos = event.motion.x;
						frameInfo.mouseYPos = event.motion.y;
					}
					break;
				}
				//case SDL_MOUSEBUTTONDOWN:
				//{
				//	if (!mouseRelativeMode)
				//	{
				//		static_cast<GUISystem *>(guiSystem)->mouseButton(event.button.button, true);
				//	}
				//	break;
				//}
				//case SDL_MOUSEBUTTONUP:
				//{
				//	if (!mouseRelativeMode)
				//	{
				//		static_cast<GUISystem *>(guiSystem)->mouseButton(event.button.button, false);
				//	}
				//	break;
				//}
				case SDL_KEYDOWN:
				{
					SDL_Keycode keyCode = event.key.keysym.scancode;
					KeyCode code = static_cast<KeyCode>(event.key.keysym.sym);

					KeyInputEvent event(code, ButtonState::DOWN);
					frameInfo.keyInputEvents.addEvent(event);

					//static_cast<GUISystem *>(guiSystem)->keyEvent(event.key.keysym.sym, true);
					//static_cast<GUISystem *>(guiSystem)->keyEvent(event.key.keysym.sym, true);
					//static_cast<GUISystem *>(guiSystem)->keyMods(event.key.keysym.mod);

					//SDL_Keycode keyCode = event.key.keysym.sym;

					break;
				}
				case SDL_KEYUP:
				{
					SDL_Keycode keyCode = event.key.keysym.scancode;
					KeyCode code = static_cast<KeyCode>(event.key.keysym.sym);

					KeyInputEvent event(code, ButtonState::UP);
					frameInfo.keyInputEvents.addEvent(event);
					//if (event.key.keysym.sym == SDLK_BACKQUOTE)
					//{
					//	setMouseRelativeMode(!mouseRelativeMode);
					//}
					//static_cast<GUISystem *>(guiSystem)->keyEvent(event.key.keysym.sym, false);

					//SDL_Keycode keyCode = event.key.keysym.sym;
					//KeyInputEvent event(keyCode, ButtonState::UP);
					//frameInfo.keyInputEvents.addEvent(event);

					break;
				}
				//case SDL_TEXTINPUT:
				//{
				//	static_cast<GUISystem *>(guiSystem)->textInput(event.text.text);
				//	break;
				//}
			}
		}
	}
};


int main(int argc, char *argv[])
{
	Size initialSize(1920, 1080);
	Logger::setDestination(&logDestination);

	bool showHelp = false;
	bool enableDebug = false;
	bool useVulkan = false;

	if (argc > 1)
	{
		if (std::strcmp(argv[1], "--help") == 0)
		{
			showHelp = true;
		}
		else if (std::strcmp(argv[1], "--debug") == 0)
		{
			enableDebug = true;
		}
	}
	if (showHelp)
	{
		std::cout << "--debug : Enabled debugging and validation\n" << std::endl;
	}
	else
	{
		//SDL_Window *win = nullptr;
		//Boiler::Size initialSize(1920, 1080);

		//// create an SDL window
		//if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
		//{
		//	if (useVulkan)
		//	{
		//		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

		//		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		//			initialSize.width, initialSize.height, winFlags);
		//		if (!win)
		//		{
		//			std::cout << "Window Error: " << SDL_GetError() << std::endl;
		//			throw std::runtime_error("Unable to create main window");
		//		}
		//	}
		//	else
		//	{
		//		// Request an OpenGL 4.6 context (should be core)
		//		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		//		// Also request a depth buffer
		//		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		//		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		//		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		//			initialSize.width, initialSize.height, winFlags);
		//		if (!win)
		//		{
		//			std::cout << "Window Error: " << SDL_GetError() << std::endl;
		//			throw std::runtime_error("Unable to create main window");
		//		}

		//		SDL_GLContext glContext = SDL_GL_CreateContext(win);
		//		SDL_GL_SetSwapInterval(1);

		//		if (!gladLoadGL())
		//		{
		//			throw std::runtime_error("Error loading OpenGL proc addresses");
		//		}
		//	}
		//}
		//else
		//{
		//	throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
		//}

		//if (useVulkan)
		//{
		//	// Vulkan extensions required by SDL
		//	unsigned int extCount = 0;
		//	SDL_Vulkan_GetInstanceExtensions(win, &extCount, nullptr);
		//	std::vector<const char *> extensions;
		//	extensions.resize(extCount);
		//	SDL_Vulkan_GetInstanceExtensions(win, &extCount, extensions.data());

		//	// create the renderer
		//	auto vulkanRenderer = new Boiler::Vulkan::VulkanRenderer(extensions, enableDebug);

		//	// vulkan renderer requires surface from platform window
		//	VkSurfaceKHR surface = VK_NULL_HANDLE;
		//	SDL_Vulkan_CreateSurface(win, vulkanRenderer->getVulkanInstance(), &surface);

		//	vulkanRenderer->setSurface(surface);

		//	renderer = vulkanRenderer;
		//}
		//else
		OpenGLSDLWindow window(initialSize);
		Renderer *renderer = new OpenGLRenderer();
		renderer->initialize(initialSize);

		// setup the engine itself
		static Engine engine;
		engine.initialize(renderer);

		// create the part and start it
		auto part = std::make_shared<EditorPart>(engine);
		engine.setPart(part);

		FrameInfo frameInfo;
		while (engine.isRunning())
		{
			window.processEvents(engine, frameInfo);
			engine.step(frameInfo);
			window.endFrame();
		}

		engine.shutdown();
		delete renderer;
	}
	return 0;
}
