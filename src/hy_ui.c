float pad = 15.0f;
float border = 3.0f;
float ch = 35.0f;

typedef struct HyFrame     HyFrame;
typedef struct HyUiContext HyUiContext;

typedef enum HyUiDirection { HyUiDirection_H, HyUiDirection_V } HyUiDirection;

struct HyUiContext {
  HyFrame* current_frame;
  HyFrame* root_frame;
  uint32_t cursor_x;
  uint32_t cursor_y;

  // Spacing
  uint32_t frame_padding;
  uint32_t button_padding;
  uint32_t inner_padding;

  // Theme
  HyColor frame_bg;

  uint32_t direction;
};

struct HyFrame {
  float width; // Internally or externally for
  float height;

  bool fixedWidth;  // If true you must also set the width
  bool fixedHeight; // If true you must also set the height

  float posY;
  float posX;

  HyFrame* children;
  uint32_t children_count;
};

global_variable HyUiContext g_ui_context;
global_variable HyFrame     g_frames[32];

internal void hyui_init()
{
  // Spacing
  g_ui_context.frame_padding = 12;
  g_ui_context.button_padding = 8;
  g_ui_context.inner_padding = 8;

  // Theme
  g_ui_context.frame_bg = hex_to_HyColor(bg0);

  g_ui_context.root_frame = &g_frames[0];
  g_ui_context.current_frame = &g_frames[0];
}

internal void hyui_begin(const char* name, uint32_t width, uint32_t height)
{
  g_ui_context.cursor_x = g_ui_context.frame_padding;
  g_ui_context.cursor_y = height - g_ui_context.frame_padding;
  g_ui_context.direction = HyUiDirection_V;
}

internal void hyui_end()
{
}

internal void hyui_begin_row()
{
  g_ui_context.direction = HyUiDirection_H;
}

internal void hyui_end_row()
{
}

internal void hyui_text(const char* label)
{
  g_ui_context.cursor_y -= FONT_SIZE;
  draw_debug_text(label, g_ui_context.cursor_x, g_ui_context.cursor_y, hex_to_HyColor(fg));

  g_ui_context.cursor_y -= g_ui_context.inner_padding;
}

internal void hyui_button(const char* label)
{
  g_ui_context.cursor_y -= FONT_SIZE;
  g_ui_context.cursor_y -= g_ui_context.button_padding * 2;
  draw_quad_2c((vec2){g_ui_context.cursor_x, g_ui_context.cursor_y},
               (vec2){200, FONT_SIZE + g_ui_context.button_padding * 2}, g_ui_context.frame_bg);
  draw_debug_text(label, g_ui_context.cursor_x + g_ui_context.button_padding,
                  g_ui_context.cursor_y + g_ui_context.button_padding, hex_to_HyColor(fg));

  g_ui_context.cursor_y -= g_ui_context.inner_padding;
}

internal void hyui_icon_button(HyTexture* icon)
{
  g_ui_context.cursor_y -= FONT_SIZE;
  g_ui_context.cursor_y -= g_ui_context.button_padding * 2;

  draw_quad_2c((vec2){g_ui_context.cursor_x, g_ui_context.cursor_y},
               (vec2){FONT_SIZE + g_ui_context.button_padding * 2, FONT_SIZE + g_ui_context.button_padding * 2},
               g_ui_context.frame_bg);
  draw_quad_2tc(g_ui_context.cursor_x + g_ui_context.button_padding,
                g_ui_context.cursor_y + g_ui_context.button_padding, (vec2){FONT_SIZE, FONT_SIZE}, icon, HyWhite);

  g_ui_context.cursor_y -= g_ui_context.inner_padding;
}

internal void hyui_render()
{
}

#if 0
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
#endif
