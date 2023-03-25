#include "core/engine.h"
#include "core/logger.h"
#include "display/vulkanrenderer.h"
#include "mainwindow.h"
#include "boileredapplication.h"
#include "tabledestination.h"
#include "gamepart.h"
#include <memory>

using namespace Boiler;

// configure vulkan renderer
const std::vector<const char *> extensions =
{
	"VK_KHR_surface",
	"VK_MVK_macos_surface"
};

int main(int argc, char *argv[])
{
	TableDestination tableDestination;
	Logger::setDestination(&tableDestination);

	BoileredApplication app(argc, argv);
	Boiler::Vulkan::VulkanRenderer renderer(extensions, true);
	renderer.setOwnedSurface(false);
	Boiler::Engine engine(&renderer);

	auto part = std::make_shared<GamePart>(engine);
	MainWindow mainWindow(engine, part, tableDestination.tableModel);
	mainWindow.showMaximized();

    app.exec();

	engine.shutdown();

	return 0;
}
