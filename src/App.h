#ifndef _APP_H
class App{
    RenderTexture2D framebuffer;
    int option;
    Selection* selection;
    Camera2D camera;
    
    int canvas_width;
    int canvas_height;
    Vector2 canvas_pos;
    
    struct{
        char* string;
        bool active;
    } text_box_data_width, text_box_data_height;
    
    Tool* tools[6];
    
    public:
    App(){
        SetCanvasSize(1280,720);
        
        camera.zoom = 0.25f;
        camera.offset = Vector2{(1280-canvas_width*camera.zoom)/2.f, (720-canvas_height*camera.zoom)/2.f};
        
        tools[0] = new Brush;
        tools[1] = new Move;
        tools[2] = new RectSelection;
        tools[3] = new CircleSelection;
        tools[4] = new BezierSelection;
        
        text_box_data_width.string = (char*)malloc(50);
        text_box_data_height.string = (char*)malloc(50);
        memset(text_box_data_width.string, 0, 50);
        memset(text_box_data_height.string, 0, 50);
    }
    
    void Update(){
        Vector2 mouse_pos = GetMousePosition();
        
        float wheel = GetMouseWheelMove();
        const float zoom_increment = 2/3.f;;
        
        if(wheel){
            camera.target = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            
            camera.zoom *= 1.5*zoom_increment + wheel*0.5f*zoom_increment;
            camera.zoom = Clamp(1/8.f, camera.zoom, 8.f);
        }
        
        BeginMode2D(camera);
        
        DrawTextureEx(framebuffer.texture, Vector2Zero(), 0, 1.f, WHITE);
        selection->DrawMoveFramebuffer();
        
        EndMode2D();
        
        selection->DrawSelection(camera, canvas_pos, canvas_width, canvas_height);
        
        DrawRectangle(0, 0, 300, 720, SKYBLUE);
        DrawRectangle(1280-300, 0, 300, 720, SKYBLUE);
        
        tools[option]->Do(*selection, framebuffer, camera, canvas_width, canvas_height, canvas_pos);
        
        GuiToggleGroup(Rectangle{0, 100, 100, 100}, "brush;move\nselect rect;select circle\nselect bezier;gradient", &option);
        
        Rectangle text_box_rect1 = Rectangle{100, 430, 150, 50};
        Rectangle text_box_rect2 = Rectangle{100, 480, 150, 50};
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            text_box_data_width.active = 0;
            text_box_data_height.active = 0;
            if(CheckCollisionPointRec(GetMousePosition(), text_box_rect1)){
                text_box_data_width.active = 1;
            }
            else if(CheckCollisionPointRec(GetMousePosition(), text_box_rect2)){
                text_box_data_height.active = 1;
            }
        }
        DrawText("width: ", 10, 445, 20, WHITE);
        if(GuiTextBox(text_box_rect1, text_box_data_width.string, 30, text_box_data_width.active)){
            text_box_data_width.active = 0;
        }
        DrawText("height: ", 10, 495, 20, WHITE);
        if(GuiTextBox(text_box_rect2, text_box_data_height.string, 30, text_box_data_height.active)){
            text_box_data_height.active = 0;
        }
        
        if(GuiButton(Rectangle{0, 550, 200, 50}, "new")){
            char char1 = text_box_data_width.string[0];
            char char2 = text_box_data_height.string[0];
            if(char1 >= '0' && char1 <= '9' && char2 >= '0' && char2 <= '9'){
                int width = atoi(text_box_data_width.string);
                int height = atoi(text_box_data_height.string);
                width = Clamp(width, 0, 1920);
                height = Clamp(width, 0, 1280);
                SetCanvasSize(width, height);
                
                sprintf(text_box_data_width.string, "%d", canvas_width);
                sprintf(text_box_data_height.string, "%d", canvas_height);
            }
        }
        
        if(GuiButton(Rectangle{0, 600, 200, 50}, "load")){
            char path[1024];
            WindowsExplorerLoad(path);
            if(path && path[0]){
                Image image = LoadImage(path);
                SetCanvasSize(image.width, image.height);
                Texture2D texture = LoadTextureFromImage(image);
                BeginTextureMode(framebuffer);
                ClearBackground(WHITE);
                DrawTextureExFlipV(texture, Vector2Zero(), 0, 1.f, WHITE);
                EndTextureMode();
            }
        }
        
        if(GuiButton(Rectangle{0, 650, 200, 50}, "save")){
            char path[1024];
            WindowsExplorerSave(path);
            if(path && path[0]){
                Image image = LoadImageFromTexture(framebuffer.texture);
                ExportImage(image, path);
            }
        }
        
        if(IsKeyReleased(KEY_DELETE) ){
            selection->DeleteSelection(framebuffer, canvas_width, canvas_height);
        }
    }
    
    void SetCanvasSize(int width, int height){
        if(IsRenderTextureReady(framebuffer)){
            UnloadRenderTexture(framebuffer);
        }
        
        canvas_width = width;
        canvas_height = height;
        canvas_pos.x = width - canvas_width;
        canvas_pos.y = height - canvas_height;
        
        if(tools[5]){
            delete tools[5];
        }
        tools[5] = new Gradient(canvas_width, canvas_height);
        
        if(selection){
            delete selection;
        }
        selection = new Selection(canvas_width, canvas_height);
        
        framebuffer = LoadRenderTexture(canvas_width, canvas_height);
        BeginTextureMode(framebuffer);
        ClearBackground(Color{255, 255, 255, 255});
        EndTextureMode();
    }
};

#define _APP_H
#endif
