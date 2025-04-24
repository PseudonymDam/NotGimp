#ifndef _MOVE_H

class Move : public Tool{
    Vector2 start_pos;
    Vector2 end_pos;
    bool is_draging;
    
    public:
    Move(){}
    
    void Do(Selection& selection, RenderTexture2D& framebuffer, Camera2D camera, float canvas_width, float canvas_height, Vector2 canvas_pos){
        Vector2 mouse_pos = GetMousePosition();
        
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_pos.x > 300 && mouse_pos.x < 1280-300){
            BeginTextureMode(selection.move_framebuffer);
            ClearBackground(Color{0, 0, 0, 0});
            BeginShaderMode(selection.move_shader);
            SetShaderValueTexture(selection.move_shader, selection.move_shader_mask, selection.GetTexture());
            SetShaderValueTexture(selection.move_shader, selection.move_shader_text, framebuffer.texture);
            
            DrawRectangle(0, 0, canvas_width, canvas_height, GREEN);
            
            EndShaderMode();
            EndTextureMode();
            
            BeginTextureMode(framebuffer);
            BeginShaderMode(selection.move_shader);
            SetShaderValueTexture(selection.move_shader, selection.move_shader_mask, selection.GetTexture());
            SetShaderValueTexture(selection.move_shader, selection.move_shader_text, selection.white_texture);
            
            DrawRectangle(0, 0, canvas_width, canvas_height, GREEN);
            
            EndShaderMode();
            EndTextureMode();
        }
        if(DoDraging(&start_pos, &end_pos, &is_draging, camera, canvas_width, canvas_height, canvas_pos)){
            Vector2 offset = Vector2Subtract(end_pos, start_pos);
            offset.x = (int)offset.x;
            offset.y = (int)offset.y;
            
            unsigned char* new_mask = (unsigned char*)malloc(canvas_width*canvas_height);
            memset(new_mask, 0, canvas_width*canvas_height);
            
            unsigned char* source = selection.mask_data - (offset.x < 0 ? (int)offset.x : 0);
            unsigned char* destination = new_mask + (int)(offset.y*canvas_width) + (offset.x > 0 ? (int)offset.x : 0);
            
            int y_start = offset.y < 0 ? -offset.y : 0;
            int y_end = offset.y > 0 ? canvas_height - offset.y : canvas_height;
            for(int y=y_start; y<y_end; y++){
                memcpy(destination + (int)(y*canvas_width), source + (int)(y*canvas_width), MathUtils::Max(canvas_width - (int)MathUtils::Abs(offset.x), 0));
            }
            
            memcpy(selection.mask_data, new_mask, canvas_width*canvas_height);
            free(new_mask);
            
            selection.EndMaskEdit(canvas_width, canvas_height);
            
            BeginTextureMode(framebuffer);
            DrawTextureExFlipV(selection.move_framebuffer.texture, offset, 0, 1, WHITE);
            EndTextureMode();
            
            BeginTextureMode(selection.move_framebuffer);
            ClearBackground(Color{0, 0, 0, 0});
            EndTextureMode();
            
            selection.mask_offset = Vector2Zero();
            start_pos.x = 0;
            start_pos.y = 0;
            end_pos.x = 0;
            end_pos.y = 0;
        }
        
        if(is_draging){
            Vector2 offset = Vector2Subtract(end_pos, start_pos);
            selection.mask_offset = offset;
            
            offset.x -= canvas_pos.x; 
            offset.y =  -offset.y-1 + canvas_pos.y;
            
            if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)){
                BeginTextureMode(framebuffer);
                DrawTextureExFlipV(selection.move_framebuffer.texture, offset, 0, 1, WHITE);
                EndTextureMode();
            }
        }
    }
};

#define _MOVE_H

#endif //_MOVE_H
