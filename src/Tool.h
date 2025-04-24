#ifndef _TOOL_H
#define MAX_BEZIER_POINT 1024

class Tool{
    public:
    virtual void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos) = 0;
    
    bool DoDraging(Vector2* start_pos, Vector2* end_pos, bool* is_draging, Camera2D camera, int canvas_width, int canvas_height, Vector2 canvas_pos){
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_pos_in_world =  GetScreenToWorld2D(mouse_pos, camera);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_pos.x > 300 && mouse_pos.x < 1280-300){
            start_pos->x = mouse_pos_in_world.x;
            start_pos->y = mouse_pos_in_world.y;
            *is_draging = 1;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && *is_draging){
            end_pos->x = mouse_pos_in_world.x;
            end_pos->y = mouse_pos_in_world.y;
        }
        
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && *is_draging){
            start_pos->x -= canvas_pos.x; 
            start_pos->y =  canvas_height-start_pos->y-1 + canvas_pos.y;
            end_pos->x -= canvas_pos.x; 
            end_pos->y = canvas_height-end_pos->y-1 + canvas_pos.y;
            
            *is_draging = 0;
            return 1;
        }
        return 0;
    }
};

#include "Brush.h"
#include "Move.h"
#include "RectSelection.h"
#include "CircleSelection.h"
#include "BezierSelection.h"
#include "Gradient.h"
#define _TOOL_H
#endif
