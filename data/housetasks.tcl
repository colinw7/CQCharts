set model [load_model -data data/housetasks.txt -first_line_header -first_column_header]

set plot [create_plot -model $model -type image]

set_charts_property -plot $plot -name labels.x.visible -value 1
set_charts_property -plot $plot -name labels.y.visible -value 1
set_charts_property -plot $plot -name balloon.visible -value 1
