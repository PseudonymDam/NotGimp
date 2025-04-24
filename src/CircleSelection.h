#ifndef _CIRCLE_SELECTION_H

class CircleSelection : public Tool{
    Vector2 start_pos;
    Vector2 end_pos;
    bool is_draging;
    
    public:
    CircleSelection(){}
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        Vector2 size = Vector2Subtract(end_pos, start_pos);
        size.x = MathUtils::Abs(size.x);
        size.y = MathUtils::Abs(size.y);
        float radius = Vector2Length(size);
        
        if(DoDraging(&start_pos, &end_pos, &is_draging, camera, canvas_width, canvas_height, canvas_pos)){
            size = Vector2Subtract(end_pos, start_pos);
            size.x = MathUtils::Abs(size.x);
            size.y = MathUtils::Abs(size.y);
            radius = Vector2Length(size);
            
            selection.BeginMaskEdit(canvas_width, canvas_height);
            
            if(radius){
                int x = (int)(radius+0.5f);
                int y = 0;
                selection.MaskLine(Vector2{start_pos.x-x, start_pos.y}, 2*x+1, canvas_width, canvas_height);
                selection.MaskPixel(Vector2{start_pos.x, start_pos.y-x}, canvas_width, canvas_height);
                selection.MaskPixel(Vector2{start_pos.x, start_pos.y+x}, canvas_width, canvas_height);
                while(y < x){
                    y++;
                    float equation_left = radius*radius - ((x-1)*(x-1) + y*y);
                    float equation_right = radius*radius - (x*x + y*y);
                    if(equation_left < 0){
                        x--;
                    }
                    selection.MaskLine(Vector2{start_pos.x - x, start_pos.y + y}, 2*x+1, canvas_width, canvas_height);
                    selection.MaskLine(Vector2{start_pos.x - x, start_pos.y - y}, 2*x+1, canvas_width, canvas_height);
                    selection.MaskLine(Vector2{start_pos.x - y, start_pos.y + x}, 2*y+1, canvas_width, canvas_height);
                    selection.MaskLine(Vector2{start_pos.x - y, start_pos.y - x}, 2*y+1, canvas_width, canvas_height);
                }
            }
            selection.EndMaskEdit(canvas_width, canvas_height);
            
            start_pos.x = 0;
            start_pos.y = 0;
            end_pos.x = 0;
            end_pos.y = 0;
        }
        
        size = Vector2Subtract(end_pos, start_pos);
        size.x = MathUtils::Abs(size.x);
        size.y = MathUtils::Abs(size.y);
        radius = Vector2Length(size);
        if(start_pos.x != 0){
            BeginMode2D(camera);
            DrawCircleLines(start_pos.x, start_pos.y, radius, BLACK);
            EndMode2D();
        }
        
        selection.DisplayOptions();
    }
};

#define _CIRCLE_SELECTION_H

#endif //_CIRCLE_SELECTION_H
