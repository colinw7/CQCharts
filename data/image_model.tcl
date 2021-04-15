set_charts_data -name path_list -value [list data .]

set model [load_charts_model -csv data/distrib_color.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{integer}}
set_charts_data -model $model -column 1 -name column_type -value {{color}}
set_charts_data -model $model -column 2 -name column_type -value {{image}}

show_charts_manage_models_dlg
