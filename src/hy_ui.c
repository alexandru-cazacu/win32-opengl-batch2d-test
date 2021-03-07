float pad = 15.0f;
float border = 3.0f;
float ch = 35.0f;

typedef struct HyFrame HyFrame;

struct HyFrame {
    float width; // Internally or externally for
    float height;
    bool  fixedWidth;  // If true you must also set the width
    bool  fixedHeight; // If true you must also set the height
    
    float padding;
    
    HyColor  color;
    float    posY;
    float    posX;
    HyFrame* left;
    HyFrame* right;
};

global_variable HyFrame g_rootFrame;

internal void hui_begin_row()
{
    
}

internal void hui_end_row()
{
    
}

internal void hui_begin_col()
{
    
}

internal void hui_end_col()
{
    
}

internal void hy_draw_ui_layout(HyFrame* frame)
{
    if (!frame)
        return;
    
    draw_quad_2c((vec2){frame->posX, frame->posY}, (vec2){frame->width, frame->height}, frame->color);
    
    if (frame->left != NULL)
        hy_draw_ui_layout(frame->left);
    if (frame->right != NULL)
        hy_draw_ui_layout(frame->right);
}

internal void hy_update_ui_layout(HyFrame* frame, float width, float height, float x, float y)
{
    frame->width = width;
    frame->height = height;
    frame->posX = x;
    frame->posY = y;
    
    if (!frame->left && !frame->right) {
        return;
    }
    
    if (frame->left && !frame->right) {
        hy_update_ui_layout(frame->left, width, height, x, y);
        return;
    }
    
    if (!frame->left && frame->right) {
        hy_update_ui_layout(frame->right, width, height, x, y);
        return;
    }
    
    float remainingWidth = 0.0f;
    if (frame->left->fixedWidth) {
        remainingWidth = width - frame->left->width;
        
        hy_update_ui_layout(frame->left, frame->left->width, height, x, y);
        hy_update_ui_layout(frame->right, remainingWidth, height, x + frame->left->width, y);
    } else if (frame->right->fixedWidth) {
        remainingWidth = width - frame->right->width;
        
        hy_update_ui_layout(frame->left, remainingWidth, height, x, y);
        hy_update_ui_layout(frame->right, frame->right->width, height, x + frame->left->width, y);
    } else {
        hy_update_ui_layout(frame->left, width / 2.0f, height, x, y);
        hy_update_ui_layout(frame->right, width / 2.0f, height, x + width / 2.0f, y);
    }
}
