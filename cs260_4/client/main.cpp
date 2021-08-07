#include "backendwrap.h"

int main()
{
	std::cout << "Compiles" << std::endl;

	window mainWindow;
	renderer mainRenderer(mainWindow);

	while (mainWindow.updatecheck())
	{
		mainWindow.update();

		mainRenderer.update();

		mainWindow.updateend();
	}

	return 0;
}