#ifndef MY_APP_CLASS
#define MY_APP_CLASS

class MyApp
{
public:
    //funcs
    explicit MyApp();
    ~MyApp();
    int Run();

private:
    //funcs 
    static LRESULT CALLBACK AppProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void InitWindow();
    void CreateControls();

    //fields
    HWND handler;

    //static fields
    static constexpr const wchar_t* appName = L"USB Monitor";
    static constexpr const wchar_t* className = L"MyAppWindowClass";
    static const int startWindowWidth = 800;
    static const int startWindowHeight = 600;
};

#endif // MY_APP_CLASS

