#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <boiler.h>
#include <display/vulkanrenderer.h>
#include <display/openglrenderer.h>

#include <glad/glad.h>

#include "editorpart.h"

Boiler::LogDestination logDestination;

int main(int argc, char *argv[])
{
	Boiler::Logger::setDestination(&logDestination);

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
		SDL_Window *win = nullptr;
		Boiler::Size initialSize(1920, 1080);

		// create an SDL window
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
		{
			if (useVulkan)
			{
				SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

				win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					initialSize.width, initialSize.height, winFlags);
				if (!win)
				{
					std::cout << "Window Error: " << SDL_GetError() << std::endl;
					throw std::runtime_error("Unable to create main window");
				}
			}
			else
			{
				// Request an OpenGL 4.5 context (should be core)
				SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
				// Also request a depth buffer
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

				SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

				win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					initialSize.width, initialSize.height, winFlags);
				if (!win)
				{
					std::cout << "Window Error: " << SDL_GetError() << std::endl;
					throw std::runtime_error("Unable to create main window");
				}

				SDL_GLContext glContext = SDL_GL_CreateContext(win);
				SDL_GL_SetSwapInterval(1);

				if (!gladLoadGL())
				{
					throw std::runtime_error("Error loading OpenGL proc addresses");
				}
			}
		}
		else
		{
			throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
		}

		Boiler::Renderer *renderer = nullptr;
		if (useVulkan)
		{
			// Vulkan extensions required by SDL
			unsigned int extCount = 0;
			SDL_Vulkan_GetInstanceExtensions(win, &extCount, nullptr);
			std::vector<const char *> extensions;
			extensions.resize(extCount);
			SDL_Vulkan_GetInstanceExtensions(win, &extCount, extensions.data());

			// create the renderer
			auto vulkanRenderer = new Boiler::Vulkan::VulkanRenderer(extensions, enableDebug);

			// vulkan renderer requires surface from platform window
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			SDL_Vulkan_CreateSurface(win, vulkanRenderer->getVulkanInstance(), &surface);

			vulkanRenderer->setSurface(surface);

			renderer = vulkanRenderer;
		}
		else
		{
			auto openglRenderer = new Boiler::OpenGLRenderer();
			renderer = openglRenderer;
		}

		// setup the engine itself
		static Boiler::Engine engine(renderer);
		engine.initialize(initialSize);

		// create the part and start it
		auto part = std::make_shared<Boiler::EditorPart>(engine);
		engine.setPart(part);

		while (engine.isRunning())
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
								const Boiler::Size newSize(static_cast<float>(event.window.data1), static_cast<float>(event.window.data2));
								renderer->resize(newSize);

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
				}
			}
			engine.step();

			SDL_GL_SwapWindow(win);
		}

		engine.shutdown();
		delete renderer;

		SDL_DestroyWindow(win);
	}
	return 0;
}
