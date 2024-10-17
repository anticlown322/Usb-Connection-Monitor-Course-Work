#include "general.h"
#include "App.h"

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    MyApp app{};
    return app.Run();
}
