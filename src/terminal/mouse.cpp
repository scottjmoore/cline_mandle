#include "mouse.h"

DragBounds compute_drag_bounds(uint32_t col_start, uint32_t row_start,
                               uint32_t col_end, uint32_t row_end) {
    DragBounds b;
    b.col_min = (col_start < col_end) ? col_start : col_end;
    b.col_max = (col_start < col_end) ? col_end : col_start;
    b.row_min = (row_start < row_end) ? row_start : row_end;
    b.row_max = (row_start < row_end) ? row_end : row_start;
    b.span_cols = b.col_max - b.col_min;
    b.span_rows = b.row_max - b.row_min;
    return b;
}
