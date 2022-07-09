# sankey link model
set model [load_charts_model -csv data/gen_from_to_size.csv -first_line_header]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{from 0} {to 1} {value 2} {attributes 3}}]

set_charts_property -plot $plot -name text.scaled    -value 1
set_charts_property -plot $plot -name edge.scaled    -value 1
set_charts_property -plot $plot -name edge.shapeType -value LINE
set_charts_property -plot $plot -name edge.width     -value 32px
set_charts_property -plot $plot -name edge.arrow     -value 0
set_charts_property -plot $plot -name edge.usePath   -value 0
set_charts_property -plot $plot -name edge.centered  -value 1
