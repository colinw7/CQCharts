#set model [load_charts_model -csv data/vis_data_1.csv -first_line_header \
# -column_type {{{0 color}} {{1 image}} {{2 symbol}} {{3 symbol_size}} {{4 font_size}}}]

set model [load_charts_model -csv data/vis_data_2.csv -first_line_header \
 -column_type {{{0 color} {mapped 1} {min 0} {max 1}}} \
 -column_type {{{1 symbol} {mapped 1} {min 0} {max 10}}} \
 -column_type {{{2 symbol_size} {mapped 1} {size_min 4} {size_max 32}}} \
 -column_type {{{3 font_size} {mapped 1} {size_min 6} {size_max 48}}}

show_charts_manage_models_dlg
