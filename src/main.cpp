// configure vulkan renderer
//const std::vector<const char *> extensions =
//{
//	"VK_KHR_surface",
//	"VK_MVK_macos_surface"
//};

#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.showMaximized();
    return app.exec();
}
