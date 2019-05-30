set model [load_charts_model -csv data/distrib_color.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{integer}}
set_charts_data -model $model -column 1 -name column_type -value {{color}}
set_charts_data -model $model -column 2 -name column_type -value {{image}}

set plot [create_charts_plot -type distribution -model $model \
  -columns {{value 0} {color 1} {image 2}}]

show_charts_create_plot_dlg
