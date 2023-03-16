#include "App.hpp"

int main()
{
	App app;

	if (app.Init() != 0)
		return -1;

	app.Update();

    return 0;
}