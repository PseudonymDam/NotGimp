#ifndef _GRADIENT_H

class Gradient : public Tool{
    Vector2 gradient_start_pos;
    Color gradient_start_color;
    Vector2 gradient_end_pos;
    Color gradient_end_color;
    struct GradientData{
        float t;
        Color color;
    };
    vector<GradientData> gradient_data;
    bool gradient_is;
    Color* gradient_color_edit;
    RenderTexture2D gradient_framebuffer;
    Shader gradient_shader;
    Texture2D gradient_texture;
    int gradient_shader_colors;
    int gradient_shader_mask;
    int gradient_granularity;
    
    Vector2 start_pos;
    Vector2 end_pos;
    bool is_draging;
    Vector2* dragable;
    float* dragable_t;
    int selected_t_index;
    
    public:
    Gradient(float canvas_width, float canvas_height){
        gradient_shader = LoadShader("shaders/gradient.vs", "shaders/gradient.fs");
        gradient_shader_colors = GetShaderLocation(gradient_shader, "colors");
        gradient_shader_mask = GetShaderLocation(gradient_shader, "mask");
        gradient_framebuffer = LoadRenderTexture(canvas_width, canvas_height);
        gradient_granularity = sqrt(canvas_width*canvas_width + canvas_height*canvas_height);
        gradient_texture = LoadTextureFromImage(GenImageColor(gradient_granularity, 1, WHITE));
        
        selected_t_index = -1;
        
        SetTextureWrap(gradient_texture, TEXTURE_WRAP_CLAMP);
    }
    ~Gradient(){
        UnloadRenderTexture(gradient_framebuffer);
        UnloadTexture(gradient_texture);
    }
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        BeginMode2D(camera);
        if(gradient_is){
            DrawTextureExFlipV(gradient_framebuffer.texture, Vector2Zero(), 0, 1.f, WHITE);
        }
        EndMode2D();
        
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_pos_in_world =  GetScreenToWorld2D(mouse_pos, camera);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_pos.x > 300 && mouse_pos.x < 1280-300){
            start_pos.x = mouse_pos_in_world.x;
            start_pos.y = mouse_pos_in_world.y;
            
            if(!gradient_is){
                gradient_start_pos = start_pos;
                gradient_end_pos = start_pos;
                gradient_start_color = Color{255, 0, 0, 255};
                gradient_end_color = Color{0, 0, 255, 255};
                
                dragable = &gradient_end_pos;
                gradient_color_edit = &gradient_end_color;
                is_draging = 1;
                gradient_is = 1;
            }
            else{
                for(int i=0; i<gradient_data.size(); i++){
                    Vector2 the_point = Vector2Lerp(gradient_start_pos, gradient_end_pos, gradient_data[i].t);
                    
                    if(CheckCollisionPointCircle(the_point, mouse_pos_in_world, 8.f/camera.zoom)){
                        dragable_t = &gradient_data[i].t;
                        selected_t_index = i;
                        gradient_color_edit = &gradient_data[i].color;
                        is_draging = 1;
                    }
                }
                
                if(!is_draging){
                    if(CheckCollisionPointCircle(mouse_pos_in_world, gradient_end_pos, 14.f/camera.zoom)){
                        dragable = &gradient_end_pos;
                        gradient_color_edit = &gradient_end_color;
                        is_draging = 1;
                    }
                    else if(CheckCollisionPointCircle(mouse_pos_in_world, gradient_start_pos, 14.f/camera.zoom)){
                        dragable = &gradient_start_pos;
                        gradient_color_edit = &gradient_start_color;
                        is_draging = 1;
                    }
                    else{
                        float the_point_t = MathUtils::PercentAlongAxis(gradient_start_pos, gradient_end_pos, mouse_pos_in_world);
                        Vector2 the_point = Vector2Lerp(gradient_start_pos, gradient_end_pos, the_point_t);
                        
                        float distance = Vector2Distance(mouse_pos_in_world, the_point);
                        if(distance < 5.f/camera.zoom && the_point_t > 0 && the_point_t < 1){
                            gradient_data.push_back({the_point_t, Color{255, 255, 0, 255}});
                            dragable_t = &gradient_data[gradient_data.size()-1].t;
                            selected_t_index = (int)gradient_data.size()-1;
                            gradient_color_edit = &gradient_data[gradient_data.size()-1].color;
                            is_draging = 1;
                            
                        }
                    }
                }
            }
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && is_draging){
            end_pos.x = mouse_pos_in_world.x;
            end_pos.y = mouse_pos_in_world.y;
            
            if(dragable){
                *dragable = end_pos;
            }
            else if(dragable_t){
                float the_point_t = MathUtils::PercentAlongAxis(gradient_start_pos, gradient_end_pos, mouse_pos_in_world);;
                
                the_point_t = Clamp(the_point_t, 0, 1);
                *dragable_t = the_point_t;
            }
        }
        
        if(selected_t_index != -1 && IsKeyReleased(KEY_DELETE)){
            gradient_data[selected_t_index] = gradient_data[gradient_data.size()-1];
            gradient_data.pop_back();
            
            dragable = 0;
            dragable_t = 0;
            is_draging = 0;
            selected_t_index = -1;
        }
        
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && is_draging){
            dragable = 0;
            dragable_t = 0;
            is_draging = 0;
        }
        
        if(gradient_is){
            {
                Vector2 dir = Vector2Subtract(gradient_end_pos, gradient_start_pos);
                if(dir.x == 0 && dir.y == 0){
                    dir.x = 1;
                }
                
                Vector2 corners[4] = {{(float)canvas_width, (float)canvas_height}, {0, (float)canvas_height}, {0, 0}, {(float)canvas_width, 0}};
                Vector2 corners_dir[4];
                for(int i =0; i<4; i++){
                    corners_dir[i] = Vector2Subtract(corners[i], gradient_start_pos);
                }
                
                int max_index = 0;
                float max = Vector2DotProduct(dir, corners_dir[0]);
                for(int i=1; i<4; i++){
                    float dot = Vector2DotProduct(dir, corners_dir[i]);
                    if(dot > max){
                        max = dot;
                        max_index = i;
                    }
                }
                
                Vector2 vecs[4];
                vecs[max_index] = Vector2{-dir.y, dir.x};
                vecs[(max_index+2)%4] = Vector2{-dir.y, dir.x};
                vecs[(max_index+1)%4] = dir;
                vecs[(max_index+3)%4] = dir;
                
                for(int i = 0; i<4; i++){
                    vecs[i] = Vector2Normalize(vecs[i]);
                    vecs[i] = Vector2Scale(vecs[i], 1000.f);
                }
                
                BeginMode2D(camera);
                
                Vector2 max_vec = Vector2{-dir.y, dir.x};
                max_vec = Vector2Normalize(max_vec);
                Vector2 max_point = corners[max_index];
                Vector2 min_point = corners[(max_index+2)%4];
                
                Vector2 point[4];
                point[0] = MathUtils::LinePerpIntersection(max_point, corners[(max_index+1)%4], max_vec);
                point[1] = MathUtils::LinePerpIntersection(max_point, corners[(max_index+3)%4], max_vec);
                point[2] = MathUtils::LinePerpIntersection(min_point, corners[(max_index+1)%4], max_vec);
                point[3] = MathUtils::LinePerpIntersection(min_point, corners[(max_index+3)%4], max_vec);
                
                Color *pixels = (Color*)malloc(sizeof(Color)*gradient_granularity);
                
                int count = (int)gradient_data.size()+2;
                Vector4* colors = (Vector4*)malloc(sizeof(Vector4)*count);
                float* ts = (float*)malloc(sizeof(float)*count);
                colors[0] = MathUtils::Color255To10(gradient_start_color);
                colors[count-1] = MathUtils::Color255To10(gradient_end_color);
                ts[0] = 0;
                ts[count-1] = 1;
                for(int i=1; i<gradient_data.size()+1; i++){
                    colors[i] = MathUtils::Color255To10(gradient_data[i-1].color);
                    ts[i] = gradient_data[i-1].t;
                }
                
                for(int i=0; i<count; i++){
                    for(int j=1; j<count; j++){
                        if(ts[j-1] > ts[j]){
                            float temp = ts[j-1];
                            ts[j-1] = ts[j];
                            ts[j] = temp;
                            
                            Vector4 color_temp = colors[j-1];
                            colors[j-1] = colors[j];
                            colors[j] = color_temp;
                        }
                    }
                }
                
                int pixels_done = 0;
                for(int i = 1; i<count; i++){
                    int pixel_count = (ts[i])*gradient_granularity - pixels_done;
                    for(int j=0; j<pixel_count; j++){
                        float t = (pixel_count-1) ? (float)j/(pixel_count-1) : 1;
                        Vector4 color_v;
                        color_v.x = (1-t)*colors[i-1].x + t*colors[i].x;
                        color_v.y = (1-t)*colors[i-1].y + t*colors[i].y;
                        color_v.z = (1-t)*colors[i-1].z + t*colors[i].z;
                        color_v.w = (1-t)*colors[i-1].w + t*colors[i].w;
                        
                        Color color = {(unsigned char)(color_v.x*255.f), (unsigned char)(color_v.y*255.f), (unsigned char)(color_v.z*255.f), (unsigned char)(color_v.w*255.f)};
                        int Index = j + pixels_done;
                        pixels[Index] = color;
                    }
                    pixels_done += pixel_count;
                }
                EndMode2D();
                
                Vector2 start_pos_ = Vector2Subtract(gradient_start_pos, canvas_pos);
                Vector2 end_pos_ = Vector2Subtract(gradient_end_pos, canvas_pos);
                
                Vector2 start_vector = Vector2Subtract(start_pos_, min_point);
                Vector2 side_point_start = Vector2Add(Vector2Scale(max_vec, Vector2DotProduct(start_vector, max_vec)), min_point);
                Vector2 end_vector = Vector2Subtract(end_pos_, max_point);
                Vector2 side_point_end = Vector2Add(Vector2Scale(max_vec, Vector2DotProduct(end_vector, max_vec)), max_point);
                
                float start_t = MathUtils::PercentAlongAxis(side_point_start, side_point_end, start_pos_);
                float end_t = MathUtils::PercentAlongAxis(side_point_start, side_point_end, end_pos_);
                
                float min_t = start_t;
                float max_t = end_t;
                
                float t0 = (max_t-min_t) ? -min_t/(max_t-min_t) : 0;
                float t1 = max_t-min_t != 0 ? (1-min_t)/(max_t-min_t) : 1;
                
                BeginTextureMode(gradient_framebuffer);
                BeginShaderMode(gradient_shader);
                UpdateTexture(gradient_texture, pixels);
                SetTextureWrap(gradient_texture, TEXTURE_WRAP_CLAMP);
                SetTextureFilter(gradient_texture, TEXTURE_FILTER_BILINEAR);
                SetShaderValueTexture(gradient_shader, gradient_shader_colors, gradient_texture);
                SetShaderValueTexture(gradient_shader, gradient_shader_mask, selection.GetTexture());
                ClearBackground(Color{0, 0, 0, 0});
                DrawPolygonGradient(point[0], point[1], point[2], point[3], t0, t1);
                EndShaderMode();
                EndTextureMode();
                free(pixels);
                free(colors);
                free(ts);
            }
            BeginMode2D(camera);
            DrawLineV(gradient_start_pos, gradient_end_pos, BLACK);
            DrawCircleV(gradient_start_pos, 14.f/camera.zoom, WHITE);
            DrawCircleV(gradient_start_pos, 12.f/camera.zoom, BLACK);
            DrawCircleV(gradient_start_pos, 10.f/camera.zoom, gradient_start_color);
            DrawCircleV(gradient_end_pos, 14.f/camera.zoom, WHITE);
            DrawCircleV(gradient_end_pos, 12.f/camera.zoom, BLACK);
            DrawCircleV(gradient_end_pos, 10.f/camera.zoom, gradient_end_color);
            
            for(int i=0; i<gradient_data.size(); i++){
                Vector2 dir = Vector2Subtract(gradient_end_pos, gradient_start_pos);
                Vector2 the_point_offset = Vector2Scale(dir, gradient_data[i].t);
                Vector2 the_point = Vector2Add(gradient_start_pos, the_point_offset);
                
                DrawCircleV(the_point, 8.f/camera.zoom, WHITE);
                DrawCircleV(the_point, 6.f/camera.zoom, BLACK);
                DrawCircleV(the_point, 4.f/camera.zoom, gradient_data[i].color);
            }
            EndMode2D();
        }
        
        if(gradient_is && IsKeyReleased(KEY_ENTER)){
            gradient_is = 0;
            is_draging = 0;
            dragable_t = 0;
            dragable = 0;
            
            float radius = canvas_width*10;
            
            BeginTextureMode(framebuffer);
            DrawTexture(gradient_framebuffer.texture, 0, 0, WHITE);
            EndTextureMode();
        }
        
        
        if(gradient_color_edit){
            GuiColorPicker(Rectangle{1280-300 + 50, 50, 200, 200}, 0, gradient_color_edit);
        }
    }
};

#define _GRADIENT_H

#endif //_GRADIENT_H
