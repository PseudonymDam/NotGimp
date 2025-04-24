#ifndef _BEZIER_SELECTION_H

class BezierSelection : public Tool{
    struct BezierPoint{
        Vector2 control1;
        Vector2 primary;
        Vector2 control2;
    };
    vector<BezierPoint> bezier_points;
    int bezier_is_closed;
    
    Vector2 start_pos = {0};
    Vector2 end_pos = {0};
    int edit_point_index = 0;
    bool is_old_point = 0;
    int control_point_nr = 0;
    bool is_editing = 0;
    
    public:
    BezierSelection(){
    }
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_pos_in_world =  GetScreenToWorld2D(mouse_pos, camera);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_pos.x > 300 && mouse_pos.x < 1280-300){
            start_pos.x = mouse_pos_in_world.x;
            start_pos.y = mouse_pos_in_world.y;
            
            if(bezier_points.size() > 0 && CheckCollisionPointCircle(start_pos, bezier_points[0].primary, 6.f/camera.zoom) && IsKeyDown(KEY_LEFT_CONTROL) && edit_point_index == bezier_points.size()-1 && !bezier_is_closed){
                bezier_points.push_back(bezier_points[0]);
                edit_point_index = (int)bezier_points.size()-1;
                
                bezier_is_closed = 1;
            }
            else{
                is_old_point = 0;
                for(int i=0; i<bezier_points.size(); i++){
                    if(CheckCollisionPointCircle(start_pos, bezier_points[i].primary, 6.f/camera.zoom) && IsKeyUp(KEY_LEFT_CONTROL)){
                        control_point_nr = 0;
                        edit_point_index = i;
                        is_old_point = 1;
                        break;
                    }
                    if(CheckCollisionPointCircle(start_pos, bezier_points[i].control1, 6.f/camera.zoom)){
                        control_point_nr = 1;
                        edit_point_index = i;
                        is_old_point = 1;
                        break;
                    }
                    if(CheckCollisionPointCircle(start_pos, bezier_points[i].control2, 6.f/camera.zoom)){
                        control_point_nr = 2;
                        edit_point_index = i;
                        is_old_point = 1;
                        break;
                    }
                }
                
                if(is_old_point){
                    is_editing = 1;
                }
                
                if(!is_old_point && IsKeyUp(KEY_LEFT_CONTROL) && !bezier_is_closed){
                    BezierPoint bezier_point = {start_pos, start_pos, start_pos};
                    bezier_points.push_back(bezier_point);
                    edit_point_index = (int)bezier_points.size()-1;
                    
                    is_editing = 1;
                }
            }
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && is_editing){
            end_pos.x = mouse_pos_in_world.x;
            end_pos.y = mouse_pos_in_world.y;
            
            if(is_old_point){
                BezierPoint* bezier_point = &bezier_points[edit_point_index];
                switch(control_point_nr){
                    case 0:{
                        Vector2 delta = Vector2Subtract(end_pos, bezier_point->primary);
                        bezier_point->primary = end_pos;
                        bezier_point->control1 = Vector2Add(bezier_point->control1, delta);
                        bezier_point->control2 = Vector2Add(bezier_point->control2, delta);
                    }break;
                    case 1:{
                        bezier_point->control1 = end_pos;
                        if(IsKeyDown(KEY_LEFT_SHIFT)){
                            Vector2 control_point = Vector2Add(bezier_point->primary, Vector2Subtract(bezier_point->primary, end_pos));
                            bezier_point->control2 = control_point;
                        }
                    }break;
                    case 2:{
                        bezier_point->control2 = end_pos;
                        if(IsKeyDown(KEY_LEFT_SHIFT)){
                            Vector2 control_point = Vector2Add(bezier_point->primary, Vector2Subtract(bezier_point->primary, end_pos));
                            bezier_point->control1 = control_point;
                        }
                    }break;
                }
            }
            else{
                Vector2 control_point = Vector2Add(start_pos, Vector2Subtract(start_pos, end_pos));
                
                BezierPoint* bezier_point = &bezier_points[bezier_points.size()-1];
                bezier_point->control1 = control_point;
                bezier_point->control2 = end_pos;
            }
        }
        
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            /*
            start_pos.x -= 1280/4; 
            start_pos.y =  canvas_height-start_pos.y-1 + 720/4;
            end_pos.x -= 1280/4; 
            end_pos.y = canvas_height-end_pos.y-1 + 720/4;
            */ 
            is_old_point = 0;
            is_editing = 0;
        }
        
        if(bezier_is_closed){
            bezier_points[bezier_points.size()-1] = bezier_points[0];
        }
        
        BeginMode2D(camera);
        int count = 1 + (int)(bezier_points.size()-1)*3;
        DrawSplineBezierCubic(((Vector2*)bezier_points.data()) + 1, count, 4.f/camera.zoom, BLACK);
        
        for(int i=0; i<bezier_points.size() - bezier_is_closed; i++){
            BezierPoint bezier_point = bezier_points[i];
            
            if(i == edit_point_index){
                DrawLineEx(bezier_point.primary, bezier_point.control1, 2.f/camera.zoom, BLACK);
                DrawLineEx(bezier_point.primary, bezier_point.control2, 2.f/camera.zoom, BLACK);
                DrawCircleV(bezier_points[i].control1,  6.f/camera.zoom, MAGENTA);
                DrawCircleV(bezier_points[i].control2,  6.f/camera.zoom, MAGENTA);
            }
            DrawCircleV(bezier_points[i].primary,  6.f/camera.zoom, SKYBLUE);
            if(IsKeyDown(KEY_LEFT_CONTROL)){
                DrawLineEx(bezier_point.primary, bezier_point.control1, 2.f/camera.zoom, BLACK);
                DrawLineEx(bezier_point.primary, bezier_point.control2, 2.f/camera.zoom, BLACK);
                DrawCircleV(bezier_points[i].control1,  6.f/camera.zoom, MAGENTA);
                DrawCircleV(bezier_points[i].control2,  6.f/camera.zoom, MAGENTA);
            }
        }
        EndMode2D();
        
        if(bezier_is_closed && IsKeyReleased(KEY_ENTER)){
            Line* lines = (Line*)malloc(sizeof(Line)*24*MAX_BEZIER_POINT);
            int lines_count = 0;
            for(int i=0; i<bezier_points.size()-1; i++){
                Vector2 p1 = Vector2Subtract(bezier_points[i].primary, canvas_pos);
                Vector2 c1 = Vector2Subtract(bezier_points[i].control2, canvas_pos);
                Vector2 c2 = Vector2Subtract(bezier_points[i+1].control1, canvas_pos);
                Vector2 p2 = Vector2Subtract(bezier_points[i+1].primary, canvas_pos);
                p1.y = canvas_height-p1.y-1;
                c1.y = canvas_height-c1.y-1;
                c2.y = canvas_height-c2.y-1;
                p2.y = canvas_height-p2.y-1;
                
#define SPLINE_SEGMENT_DIVISIONS (24)
                const float step = 1.0f/SPLINE_SEGMENT_DIVISIONS;
                
                Vector2 previous = p1;
                Vector2 current = { 0 };
                float t = 0.0f;
                
                Vector2 points[2*SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };
                
                //Data i={0};
                for (int j = 1; j <= SPLINE_SEGMENT_DIVISIONS; j++)
                {
                    t = step*(float)j;
                    
                    float a = powf(1.0f - t, 3);
                    float b = 3.0f*powf(1.0f - t, 2)*t;
                    float c = 3.0f*(1.0f - t)*powf(t, 2);
                    float d = powf(t, 3);
                    
                    current.y = a*p1.y + b*c1.y + c*c2.y + d*p2.y;
                    current.x = a*p1.x + b*c1.x + c*c2.x + d*p2.x;
                    
                    lines[lines_count++] = Line{previous, current};
                    previous = current;
                }
            }
            
            selection.BeginMaskEdit(canvas_width, canvas_height);
            for(int y=0; y<canvas_height; y++){
                float* xs = (float*)malloc(sizeof(float)*1024);
                int xs_count = 0;
                for(int j=0; j<lines_count; j++){
                    Line line = lines[j];
                    float min_y = MathUtils::Min(line.point1.y, line.point2.y);
                    float max_y = MathUtils::Max(line.point1.y, line.point2.y);
                    
                    if(y >= min_y && y < max_y){
                        float t = (y - line.point1.y)/(line.point2.y - line.point1.y);
                        xs[xs_count++] = line.point1.x + (line.point2.x-line.point1.x)*t;
                    }
                }
                
                
                qsort(xs, xs_count, sizeof(float), MathUtils::CompareNumbersQSort);
                
                for(int j=0; j<xs_count; j+=2){
                    selection.MaskLine(Vector2{(float)xs[j], (float)y}, (int)xs[j+1]-(int)xs[j], canvas_width, canvas_height);
                }
                
                free(xs);
            }
            selection.EndMaskEdit(canvas_width, canvas_height);
            free(lines);
            
            bezier_points.clear();
            bezier_is_closed = 0;
            
        } 
        selection.DisplayOptions();
    }
};

#define _BEZIER_SELECTION_H

#endif //_BEZIER_SELECTION_H
