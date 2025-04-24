#ifndef _SELECTION_H
class Move;

enum SelectionType{
    SELECTION_OVERWRITE,
    SELECTION_ADD,
    SELECTION_SUB,
    SELECTION_INTERSECTION,
};

class Selection{
    bool is_active;
    unsigned char* mask_data;
    unsigned char* mask_data_save;
    vector<Line> lines;
    int type;
    
    Texture2D mask_editable;
    Texture2D mask_full;
    
    Vector2 mask_offset;
    
    RenderTexture2D move_framebuffer;
    Shader move_shader;
    int move_shader_mask;
    int move_shader_text;
    
    Texture2D white_texture;
    
    public:
    Selection(int canvas_width, int canvas_height){
        move_shader = LoadShader("shaders/shader_rect.vs", "shaders/shader_rect.fs");
        move_shader_mask = GetShaderLocation(move_shader, "mask");
        move_shader_text = GetShaderLocation(move_shader, "text");
        
        white_texture = LoadTextureFromImage(GenImageColor(1, 1, WHITE));
        
        move_framebuffer = LoadRenderTexture(canvas_width, canvas_height);
        BeginTextureMode(move_framebuffer);
        ClearBackground(Color{0, 0, 0, 0});
        EndTextureMode();
        
        unsigned char* mask_buffer = (unsigned char*)malloc(canvas_width*canvas_height);
        memset(mask_buffer, 0xff, canvas_width*canvas_height);
        Image image = GenImageText(canvas_width, canvas_height, (const char*)mask_buffer);
        mask_full = LoadTextureFromImage(image);
        mask_editable = LoadTextureFromImage(image);
        UnloadImage(image);
        free(mask_buffer);
        
        mask_data = (unsigned char*)malloc(canvas_width*canvas_height);
        mask_data_save = (unsigned char*)malloc(canvas_width*canvas_height);
        memset(mask_data, 0, canvas_width*canvas_height);
        memset(mask_data_save, 0, canvas_width*canvas_height);
    }
    
    ~Selection(){
        UnloadTexture(mask_full);
        UnloadTexture(mask_editable);
        UnloadRenderTexture(move_framebuffer);
        
        free(mask_data);
        free(mask_data_save);
    }
    
    void DisplayOptions(){
        GuiToggleGroup(Rectangle{1280-300 + 50, 0, 100, 100}, "overwrite\nadd\nsub\nintersection", &type);
    }
    
    Texture2D GetTexture(){
        return is_active ? mask_editable : mask_full;
    }
    
    void BeginMaskEdit(int canvas_width, int canvas_height){
        if(type == SELECTION_OVERWRITE){
            memset(mask_data, 0, canvas_width*canvas_height);
        }
    }
    
    void EndMaskEdit(int canvas_width, int canvas_height){
        if(type == SELECTION_INTERSECTION){
            memcpy(mask_data, mask_data_save, canvas_width*canvas_height);
            memset(mask_data_save, 0, canvas_width*canvas_height);
        }
        
        Vector2* edges = (Vector2*)malloc(canvas_width*canvas_height*sizeof(Vector2));
        int edges_count = 0;
        
        unsigned char* mask = mask_data;
        
        if(mask[0]){
            edges[edges_count++] = Vector2{0, 0};
        }
        for(int x=1; x<canvas_width; x++){
            if(mask[x] != mask[x-1]){
                edges[edges_count++] = Vector2{(float)x, 0};
            }
        }
        if(mask[canvas_width-1]){
            edges[edges_count++] = Vector2{(float)canvas_width, 0};
        }
        
        for(int y=1; y<canvas_height; y++){
            if(mask[y*canvas_width] != mask[(y-1)*canvas_width]){
                edges[edges_count++] = Vector2{0, (float)y};
            }
            for(int x=1; x<canvas_width; x++){
                int masked_pixels_count = mask[y*canvas_width + x] > 0;
                masked_pixels_count+= mask[(y-1)*canvas_width + x] > 0;
                masked_pixels_count+= mask[(y-1)*canvas_width + x-1] > 0;
                masked_pixels_count+= mask[y*canvas_width + x-1] > 0;
                
                if(masked_pixels_count == 3 || masked_pixels_count == 1){
                    edges[edges_count++] = Vector2{(float)x, (float)y};
                }
            }
            if(mask[y*canvas_width + canvas_width-1] != mask[(y-1)*canvas_width + canvas_width-1]){
                edges[edges_count++] = Vector2{(float)canvas_width, (float)y};
            }
        }
        
        if(mask[(canvas_height-1)*canvas_width]){
            edges[edges_count++] = Vector2{0, (float)canvas_height};
        }
        for(int x=1; x<canvas_width; x++){
            if(mask[(canvas_height-1)*canvas_width + x] != mask[(canvas_height-1)*canvas_width + x-1]){
                edges[edges_count++] = Vector2{(float)x, (float)canvas_height};
            }
        }
        if(mask[canvas_height*canvas_width-1]){
            edges[edges_count++] = Vector2{(float)canvas_width, (float)canvas_height};
        }
        
        lines.clear();
        
        float line_thickness = 5.f;
        for(int i=0; i<edges_count; i+=2){
            Line line;
            line.point1 = edges[i];
            line.point2 = edges[i+1];
            lines.push_back(line);
        }
        
        bool* were_edges_used = (bool*)malloc(edges_count*sizeof(bool));
        memset(were_edges_used, 0, edges_count*sizeof(bool)); 
        for(int i=0; i<edges_count; i++){
            if(were_edges_used[i]){
                continue;
            }
            
            Line line;
            for(int j=i+1; j<edges_count; j++){
                if(edges[i].x == edges[j].x){
                    line.point1 = edges[j];
                    line.point2 = edges[i];
                    lines.push_back(line);
                    
                    were_edges_used[i] = 1;
                    were_edges_used[j] = 1;
                    break;
                }
            }
        }
        
        free(edges);
        free(were_edges_used);
        
        UpdateTexture(mask_editable, mask_data);
        
        bool is_empty = 1;
        for(int i=0; i<canvas_width*canvas_height; i++){
            if(mask_data[i]){
                is_empty = 0;
                break;
            }
        }
        
        is_active = !is_empty;
    }
    
    
    void MaskPixel(Vector2 pos, int canvas_width, int canvas_height){
        if(!CheckCollisionPointRec(pos, Rectangle{0, 0, (float)canvas_width-1, (float)canvas_height-1})){
            return;
        }
        
        int index = ((int)pos.y)*canvas_width + (int)pos.x;
        if(type == SELECTION_OVERWRITE || type == SELECTION_ADD){
            mask_data[index] = 0xff;
        }
        else if(type == SELECTION_INTERSECTION){
            mask_data_save[index] = mask_data[index];
        }
        else{
            mask_data[index] = 0;
        }
    }
    
    void MaskLine(Vector2 pos, int width, int canvas_width, int canvas_height){
        if(pos.y < 0 || pos.y >= canvas_height){
            return;
        }
        
        if(pos.x < 0){
            width += pos.x;
            pos.x = 0;
        }
        pos.x = MathUtils::Min(pos.x, canvas_width-1);
        width = Clamp(width, 0, canvas_width - (int)pos.x);
        
        int index = ((int)pos.y)*canvas_width + (int)pos.x;
        if(type == SELECTION_OVERWRITE || type == SELECTION_ADD){
            memset(mask_data + index, 0xff, width);
        }
        else if(type == SELECTION_INTERSECTION){
            memcpy(mask_data_save + index, mask_data + index, width);
        }
        else{
            memset(mask_data + index, 0, width);
        }
    }
    
    void DrawMoveFramebuffer(){
        DrawTextureEx(move_framebuffer.texture, mask_offset, 0, 1.f, WHITE);
    }
    
    void DrawSelection(Camera2D camera, Vector2 canvas_pos, int canvas_width, int canvas_height){
        if(is_active){
            for(int i=0; i<lines.size(); i++){
                Line line = lines[i];
                BeginMode2D(camera);
                Vector2 start_pos;
                Vector2 end_pos;
                start_pos.x = line.point1.x + canvas_pos.x;
                start_pos.y = canvas_height - line.point1.y + canvas_pos.y;
                end_pos.x = line.point2.x + canvas_pos.x;
                end_pos.y = canvas_height - line.point2.y + canvas_pos.y;
                DrawLineEx(Vector2Add(start_pos, mask_offset), Vector2Add(end_pos, mask_offset), 1.f/camera.zoom, RED);
                EndMode2D();
            }
        }
    }
    void DeleteSelection(RenderTexture2D& framebuffer, int canvas_width, int canvas_height){
        if(is_active){
            BeginTextureMode(framebuffer);
            BeginShaderMode(move_shader);
            SetShaderValueTexture(move_shader, move_shader_mask, GetTexture());
            SetShaderValueTexture(move_shader, move_shader_text, white_texture);
            
            DrawRectangle(0, 0, canvas_width, canvas_height, GREEN);
            
            EndShaderMode();
            EndTextureMode();
        }
    }
    friend Move;
};

#define _SELECTION_H
#endif
