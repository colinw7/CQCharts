set model [load_charts_model -csv data/force_directed_one.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed -columns {{from 0} {to 1}}]

set_charts_property -plot $plot -name edge.shapeType -value ARC
set_charts_property -plot $plot -name edge.arrow     -value MID
set_charts_property -plot $plot -name edge.scaled    -value 0
