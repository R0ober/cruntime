#include "visual.h"

#include <stdio.h>
#include <string.h>

void visual_draw_horizontal(const char* left, const char* right, int width) {
    printf(COLOR_BORDER "%s" RESET, left);
    for (int i = 0; i < width; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_BORDER "%s\n" RESET, right);
}

void visual_draw_title(const char* title, int width) {
    int title_len = (int)strlen(title);
    int left_pad = (width - title_len) / 2;
    int right_pad = width - title_len - left_pad;

    printf(COLOR_BORDER BOX_TL RESET);
    for (int i = 0; i < left_pad; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_TITLE "%s" RESET, title);
    for (int i = 0; i < right_pad; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_BORDER BOX_TR RESET "\n");
}

void visual_draw_section_header(const char* title, int width) {
    char line[96];
    snprintf(line, sizeof(line), "[ %s ]", title);

    printf(COLOR_BORDER BOX_V RESET " " COLOR_GOLD "%-*s" RESET " " COLOR_BORDER BOX_V RESET "\n",
           width - 2,
           line);
    visual_draw_horizontal(BOX_LT, BOX_RT, width);
}

void visual_draw_kv_row(const char* key, size_t value, int width) {
    char val_buf[32];
    snprintf(val_buf, sizeof(val_buf), "%zu", value);

    printf(COLOR_BORDER BOX_V RESET " " COLOR_WHITE "%-24s" RESET " : " COLOR_GREEN "%10s" RESET,
           key,
           val_buf);

    int used = 1 + 24 + 3 + 10;
    for (int i = used; i < width; i++) {
        printf(" ");
    }
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_text_row(const char* text, int width, const char* color) {
    int text_len = (int)strlen(text);
    if (text_len > width - 2) {
        text_len = width - 2;
    }

    printf(COLOR_BORDER BOX_V RESET " ");
    printf("%s%.*s" RESET, color, text_len, text);

    for (int i = text_len + 1; i < width; i++) {
        printf(" ");
    }
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_table_header4(const char* c1,const char* c2,const char* c3,const char* c4,int width) {
    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_GOLD "  %-8s %-8s %-18s %-39s" RESET, c1, c2, c3, c4);
    printf(COLOR_BORDER BOX_V RESET "\n");
    visual_draw_horizontal(BOX_LT, BOX_RT, width);
}

void visual_draw_slab_row(size_t slab_size,int free_count,const void* head,int max_bar,int width) {
    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_WHITE "  %-8zu " RESET, slab_size);
    printf(COLOR_WHITE "%-8d " RESET, free_count);

    if (head != NULL) {
        printf(COLOR_ADDR "%-18p " RESET, head);
    } else {
        printf(COLOR_ADDR "%-18s " RESET, "(nil)");
    }
    visual_draw_node_block_bar(free_count, max_bar);

    (void)width;
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_summary_bytes(const char* label, size_t bytes, int width) {
    char summary_buf[96];
    int summary_len = snprintf(summary_buf, sizeof(summary_buf), "  %s: %zub", label, bytes);

    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_GOLD "  %s: " COLOR_GREEN "%zub" RESET, label, bytes);
    for (int i = summary_len; i < width; i++) {
        printf(" ");
    }
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_node_block_bar(int count, int max_bar) {
    if (max_bar <= 0) {
        return;
    }
    int overflow = count > max_bar;
    int bar_len = overflow ? (max_bar - 1) : count;
    if (bar_len < 0) {
        bar_len = 0;
    }
    for (int i = 0; i < bar_len; i++) {
        if ((i % 2) == 0) {
            printf(COLOR_FREE BLOCK_FULL RESET);
        } else {
            printf(COLOR_FREE BLOCK_EMPTY RESET);
        }
    }
    if (overflow) {
        printf(COLOR_GOLD "+" RESET);
    }
    int used_slots = bar_len + (overflow ? 1 : 0);
    for (int i = used_slots; i < max_bar; i++) {
        printf(" ");
    }
}

void visual_draw_usage_block_bar(size_t used, size_t capacity, int max_bar) {
    if (max_bar <= 0) {
        return;
    }

    int filled = 0;
    if (capacity > 0) {
        filled = (int)((used * (size_t)max_bar) / capacity);
    }
    if (filled > max_bar) {
        filled = max_bar;
    }

    for (int i = 0; i < max_bar; i++) {
        if (i < filled) {
            printf(COLOR_ALLOC BLOCK_ALLOC RESET);
        } else {
            printf(COLOR_GRAY BLOCK_EMPTY RESET);
        }
    }
}

void visual_draw_usage_row(const char* label,size_t used,size_t capacity,int max_bar,int width) {
    char bytes_buf[48];
    snprintf(bytes_buf, sizeof(bytes_buf), "%zu/%zu", used, capacity);

    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_WHITE " %-18s " RESET, label);
    visual_draw_usage_block_bar(used, capacity, max_bar);
    printf(" ");
    printf(COLOR_GOLD "%10s" RESET, bytes_buf);

    int used_cols = 1 + 18 + 1 + max_bar + 1 + 10;
    for (int i = used_cols; i < width; i++) {
        printf(" ");
    }

    printf(COLOR_BORDER BOX_V RESET "\n");
}

static void visual_print_spaces(int count) {
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

static void visual_subbox_print_columns4(const char* c1,const char* c2,const char* c3,const char* c4,
                                        int w1,int w2,int w3,int w4,int outer_width,int inset,const char* color) {
    int inner_width = outer_width - (2 * inset) - 2;
    if (inner_width < 0) {
        inner_width = 0;
    }

    const char* cols[4] = {c1, c2, c3, c4};
    int widths[4] = {w1, w2, w3, w4};

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET);

    int used = 0;
    int first = 1;
    for (int i = 0; i < 4; i++) {
        if (widths[i] <= 0) {
            continue;
        }
        if (!first) {
            printf(" ");
            used += 1;
        }
        const char* text = cols[i] ? cols[i] : "";
        printf("%s%-*.*s" RESET, color, widths[i], widths[i], text);
        used += widths[i];
        first = 0;
    }

    if (used < inner_width) {
        visual_print_spaces(inner_width - used);
    }

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_subbox_top(const char* title, int outer_width, int inset) {
    int inner_width = outer_width - (2 * inset) - 2;
    if (inner_width < 0) {
        inner_width = 0;
    }

    int title_len = (int)strlen(title);
    if (title_len > inner_width) {
        title_len = inner_width;
    }
    int left_pad = (inner_width - title_len) / 2;
    int right_pad = inner_width - title_len - left_pad;

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_TL RESET);
    for (int i = 0; i < left_pad; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_GOLD "%.*s" RESET, title_len, title);
    for (int i = 0; i < right_pad; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_BORDER BOX_TR RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_subbox_kv_row(const char* key, size_t value, int outer_width, int inset) {
    int inner_width = outer_width - (2 * inset) - 2;
    if (inner_width < 0) {
        inner_width = 0;
    }

    char line[128];
    int line_len = snprintf(line, sizeof(line), " %-24s : %10zu", key, value);
    if (line_len < 0) {
        line_len = 0;
    }
    if (line_len > inner_width) {
        line_len = inner_width;
    }

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_WHITE "%.*s" RESET, line_len, line);
    visual_print_spaces(inner_width - line_len);
    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_subbox_divider(int outer_width, int inset) {
    int inner_width = outer_width - (2 * inset) - 2;
    if (inner_width < 0) {
        inner_width = 0;
    }

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_LT RESET);
    for (int i = 0; i < inner_width; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_BORDER BOX_RT RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET "\n");
}

void visual_draw_subbox_columns_header4(const char* c1,const char* c2,const char* c3,const char* c4,
                                        int w1,int w2,int w3,int w4,int outer_width,int inset) {
    visual_subbox_print_columns4(c1,c2,c3,c4,
                                 w1,w2,w3,w4,outer_width,inset,COLOR_GOLD);
    visual_draw_subbox_divider(outer_width, inset);
}

void visual_draw_subbox_columns_row4(const char* v1,const char* v2,const char* v3,const char* v4,
                                     int w1,int w2,int w3,int w4,int outer_width,int inset) {
    visual_subbox_print_columns4(v1,v2,v3,v4,
                                w1,w2,w3,w4,outer_width,inset,COLOR_WHITE);
}

void visual_draw_subbox_bottom(int outer_width, int inset) {
    int inner_width = outer_width - (2 * inset) - 2;
    if (inner_width < 0) {
        inner_width = 0;
    }

    printf(COLOR_BORDER BOX_V RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_BL RESET);
    for (int i = 0; i < inner_width; i++) {
        printf(COLOR_BORDER BOX_H RESET);
    }
    printf(COLOR_BORDER BOX_BR RESET);
    visual_print_spaces(inset);
    printf(COLOR_BORDER BOX_V RESET "\n");
}
