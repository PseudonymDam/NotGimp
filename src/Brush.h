#ifndef _BRUSH_H

class Brush : public Tool{
    struct{
        Shader shader;
        int pos;
        int radius;
        int mask;
    } shader_data;
    float size;
    Color brush_color;
    Vector2 previous_draw_pos;
    public:
    Brush(){
        shader_data.shader = LoadShader("shaders/shader.vs", "shaders/shader.fs");
        shader_data.radius = GetShaderLocation(shader_data.shader, "radius");
        shader_data.mask = GetShaderLocation(shader_data.shader, "mask");
        brush_color = RED;
        size = 15;
    }
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        Vector2 world_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        canvas_pos = Vector2{world_pos.x - canvas_pos.x, canvas_height-(world_pos.y - canvas_pos.y)};
        
        
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            BeginTextureMode(framebuffer);
            BeginShaderMode(shader_data.shader);
            SetShaderValueTexture(shader_data.shader, shader_data.mask, selection.GetTexture());
            SetShaderValue(shader_data.shader, shader_data.radius, (const void*)&size, RL_SHADER_UNIFORM_FLOAT);
            
            DrawCircleSmooth(canvas_pos, size, brush_color);
            
            EndShaderMode();
            EndTextureMode(); 
            
            previous_draw_pos = canvas_pos;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 circle_pos = canvas_pos;
            float length = Vector2Length(Vector2Subtract(previous_draw_pos, circle_pos));
            
            if((int)length){
                BeginTextureMode(framebuffer);
                for(int i=0; i<(int)length; i++){
                    float t = (float)i/(int)length;
                    Vector2 pos = Vector2Lerp(previous_draw_pos, circle_pos, t);
                    
                    BeginShaderMode(shader_data.shader);
                    SetShaderValueTexture(shader_data.shader, shader_data.mask, selection.GetTexture());
                    SetShaderValue(shader_data.shader, shader_data.radius, (const void*)&size, RL_SHADER_UNIFORM_FLOAT);
                    
                    DrawCircleSmooth(pos, size, brush_color);
                    
                    EndShaderMode();
                }
                EndTextureMode();
                previous_draw_pos = circle_pos;
            }
        }
        
        Vector2 mouse_pos = GetMousePosition();
        DrawCircleLines(mouse_pos.x, mouse_pos.y, size*camera.zoom, brush_color);
        
        
        GuiSlider(Rectangle{1280-300 + 50, 0, 100, 50}, "Size", 0, &size, 0, 30);
        GuiColorPicker(Rectangle{1280-300 + 50, 150, 200, 200}, 0, &brush_color);
    }
};

#define _BRUSH_H

#endif //_BRUSH_H
