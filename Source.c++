#include "Application.h++"

int main()
{
    try 
    {
        Application app;
        app.run();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Engine Initialization Failed: " << e.what() << '\n';
        return -1;
    }

    return 0;
}