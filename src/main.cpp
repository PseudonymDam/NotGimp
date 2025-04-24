#include "src/raylib.h"
#include <sys/stat.h>
#include <time.h>
#include <vector>
#include <math.h>
#include "raymath.h"

using namespace std;

char* WindowsExplorerLoad(char*);
char* WindowsExplorerSave(char*);

#define SUPPORT_FILEFORMAT_JPG
#include "rlgl.h"

//hack to fix problem with raygui, will not compile without that
static float TextToFloat(const char *text)
{
    float value = 0.0f;
    float sign = 1.0f;
    
    if ((text[0] == '+') || (text[0] == '-'))
    {
        if (text[0] == '-') sign = -1.0f;
        text++;
    }
    
    int i = 0;
    for (; ((text[i] >= '0') && (text[i] <= '9')); i++) value = value*10.0f + (float)(text[i] - '0');
    
    if (text[i++] != '.') value *= sign;
    else
    {
        float divisor = 10.0f;
        for (; ((text[i] >= '0') && (text[i] <= '9')); i++)
        {
            value += ((float)(text[i] - '0'))/divisor;
            divisor = divisor*10.0f;
        }
    }
    
    return value;
}


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "MathUtils.h"
#include "Selection.h"
#include "Tool.h"
#include "App.h"

int main()
{
    int screenWidth = 1280;
    int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Project");
    
    SetTargetFPS(60);
    
    App* app = new App();
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        ClearBackground(Color{150, 150, 150, 255});
        
        rlSetBlendFactorsSeparate(RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, RL_ONE, RL_ONE, RL_FUNC_ADD, RL_MAX);
        
        BeginBlendMode(RL_BLEND_CUSTOM_SEPARATE);
        app->Update();
        EndBlendMode(); 
        
        EndDrawing(); 
    }
    
    CloseWindow();
    return 0;
}