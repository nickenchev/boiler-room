#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <boiler.h>
#include <display/vulkanrenderer.h>

#include "gamepart.h"

Boiler::LogDestination logDestination;

int main(int argc, char *argv[])
{
	Boiler::Logger::setDestination(&logDestination);

	bool showHelp = false;
	bool enableDebug = false;
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
		Boiler::Size initialSize(1920, 1080);

		// create an SDL window
		SDL_Window *win = nullptr;
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
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
			throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
		}

		// Vulkan extensions required by SDL
		unsigned int extCount = 0;
		SDL_Vulkan_GetInstanceExtensions(win, &extCount, nullptr);
		std::vector<const char *> extensions;
		extensions.resize(extCount);
		SDL_Vulkan_GetInstanceExtensions(win, &extCount, extensions.data());

		// create the renderer and surface
		auto renderer = std::make_unique<Boiler::Vulkan::VulkanRenderer>(extensions, enableDebug);
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		SDL_Vulkan_CreateSurface(win, renderer->getVulkanInstance(), &surface);
		renderer->setSurface(surface);

		// setup the engine itself
		static Boiler::Engine engine(renderer.get());
		engine.initialize(initialSize);

		// create the part and start it
		auto part = std::make_shared<GamePart>(engine);
		engine.setPart(part);

		while (engine.isRunning())
		{
			engine.step();
		}
		engine.shutdown();

		SDL_DestroyWindow(win);
	}
	return 0;
}
