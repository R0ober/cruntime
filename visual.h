#pragma once

#include <stddef.h>

// terminal visuals
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define DIM       "\033[2m"

// Gruvbox palette
#define COLOR_WHITE       "\033[38;5;223m"   // gruvbox fg
#define COLOR_GOLD        "\033[38;5;214m"   // gruvbox yellow
#define COLOR_GRAY        "\033[38;5;245m"   // gruvbox gray
#define COLOR_CORAL       "\033[38;5;167m"   // gruvbox red
#define COLOR_GREEN       "\033[38;5;142m"   // gruvbox green
#define COLOR_LIGHT_GREY  "\033[38;5;250m"   // light foreground

#define COLOR_ALLOC   COLOR_CORAL
#define COLOR_FREE    COLOR_GREEN
#define COLOR_TITLE   COLOR_GOLD BOLD
#define COLOR_BORDER  COLOR_GRAY
#define COLOR_ADDR    COLOR_LIGHT_GREY

#define BLOCK_FULL    "█"
#define BLOCK_EMPTY   "░"
#define BLOCK_ALLOC   "█"

#define SEP    "│"
#define BOX_TL "╭"
#define BOX_TR "╮"
#define BOX_BL "╰"
#define BOX_BR "╯"
#define BOX_H  "─"
#define BOX_V  "│"
#define BOX_LT "├"
#define BOX_RT "┤"

void visual_draw_horizontal(const char* left, const char* right, int width);
void visual_draw_title(const char* title, int width);
void visual_draw_section_header(const char* title, int width);
void visual_draw_kv_row(const char* key, size_t value, int width);
void visual_draw_text_row(const char* text, int width, const char* color);
void visual_draw_table_header4(const char* c1,
							   const char* c2,
							   const char* c3,
							   const char* c4,
							   int width);
void visual_draw_slab_row(size_t slab_size,
						  int free_count,
						  const void* head,
						  int max_bar,
						  int width);
void visual_draw_summary_bytes(const char* label, size_t bytes, int width);
void visual_draw_node_block_bar(int count, int max_bar);
void visual_draw_usage_block_bar(size_t used, size_t capacity, int max_bar);
void visual_draw_usage_row(const char* label,
						   size_t used,
						   size_t capacity,
						   int max_bar,
						   int width);

void visual_draw_subbox_top(const char* title, int outer_width, int inset);
void visual_draw_subbox_kv_row(const char* key, size_t value, int outer_width, int inset);
void visual_draw_subbox_divider(int outer_width, int inset);
void visual_draw_subbox_columns_header4(const char* c1,
										const char* c2,
										const char* c3,
										const char* c4,
										int w1,
										int w2,
										int w3,
										int w4,
										int outer_width,
										int inset);
void visual_draw_subbox_columns_row4(const char* v1,
									 const char* v2,
									 const char* v3,
									 const char* v4,
									 int w1,
									 int w2,
									 int w3,
									 int w4,
									 int outer_width,
									 int inset);
void visual_draw_subbox_bottom(int outer_width, int inset);
