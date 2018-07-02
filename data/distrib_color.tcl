set model [load_model -csv data/distrib_color.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value integer

set plot [create_plot -model $model -columns "group=0,color=1"]

create_plot_dlg
