#include "MainApp\Headers\App\general.h"
#include "MainApp\Headers\App\App.h"

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    MyApp app{};
    return app.Run();
}
