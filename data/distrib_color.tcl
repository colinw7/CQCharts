set model [load_model -csv data/distrib_color.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value integer

create_plot_dlg
