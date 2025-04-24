#ifndef _RECT_SELECTION_H

class RectSelection : public Tool{
    Vector2 start_pos = {0};
    Vector2 end_pos = {0};
    bool is_draging = 0;
    
    public:
    RectSelection(){}
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        Vector2 pos = Vector2{MathUtils::Min(start_pos.x, end_pos.x), MathUtils::Min(start_pos.y, end_pos.y)};
        Vector2 size = Vector2Subtract(end_pos, start_pos);
        size.x = MathUtils::Abs(size.x);
        size.y = MathUtils::Abs(size.y);
        if(is_draging){
            BeginMode2D(camera);
            DrawLineEx(pos, Vector2Add(pos, Vector2{size.x, 0}), 1.f/camera.zoom, RED);
            DrawLineEx(pos, Vector2Add(pos, Vector2{0, size.y}), 1.f/camera.zoom, RED);
            DrawLineEx(Vector2Add(pos, size), Vector2Add(pos, Vector2{size.x, 0}), 1.f/camera.zoom, RED);
            DrawLineEx(Vector2Add(pos, size), Vector2Add(pos, Vector2{0, size.y}), 1.f/camera.zoom, RED);
            EndMode2D();
        }
        
        if(DoDraging(&start_pos, &end_pos, &is_draging, camera, canvas_width, canvas_height, canvas_pos)){
            Vector2 pos = Vector2{MathUtils::Min(start_pos.x, end_pos.x), MathUtils::Max(start_pos.y, end_pos.y)};
            Vector2 size = Vector2Subtract(end_pos, start_pos);
            size.x = MathUtils::Abs(size.x);
            size.y = MathUtils::Abs(size.y);
            
            selection.BeginMaskEdit(canvas_width, canvas_height);
            for(int y=0; y<(int)size.y; y++){
                selection.MaskLine(Vector2{pos.x, pos.y-y}, size.x, canvas_width, canvas_height);
            }
            selection.EndMaskEdit(canvas_width, canvas_height);
            
            start_pos.x = 0;
            start_pos.y = 0;
            end_pos.x = 0;
            end_pos.y = 0;
        }
        
        selection.DisplayOptions();
    }
};

#define _RECT_SELECTION_H

#endif //_RECT_SELECTION_H
