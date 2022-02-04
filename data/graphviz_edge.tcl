set model [load_charts_model -csv data/dot_edge.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type graphviz -columns {{link 0} {value 1}}]

set_charts_property -plot $plot -name text.scaled    -value 1
set_charts_property -plot $plot -name edge.scaled    -value 1
set_charts_property -plot $plot -name edge.shapeType -value LINE
set_charts_property -plot $plot -name edge.width     -value 32px
set_charts_property -plot $plot -name edge.arrow     -value 0
set_charts_property -plot $plot -name edge.usePath   -value 0 
set_charts_property -plot $plot -name edge.centered  -value 
