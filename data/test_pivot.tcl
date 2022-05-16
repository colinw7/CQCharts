#set model [load_charts_model -csv data/Arthritis.csv -first_line_header]
set model [load_charts_model -csv data/mps.csv -first_line_header]

show_charts_manage_models_dlg

set pmodel [create_charts_pivot_model -model $model -hcolumns 2 -vcolumns 1 -dcolumns 3]
