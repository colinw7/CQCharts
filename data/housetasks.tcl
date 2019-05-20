set model [load_charts_model -data data/housetasks.txt -first_line_header -first_column_header]

set plot [create_charts_plot -model $model -type image]

set_charts_property -plot $plot -name xaxis.text.visible -value 1
set_charts_property -plot $plot -name yaxis.text.visible -value 1
set_charts_property -plot $plot -name cell.style -value BALLOON
