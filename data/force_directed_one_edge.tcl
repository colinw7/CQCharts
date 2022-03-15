# sankey link model
#set model [load_charts_model -csv data/one_edge.csv -comment_header]
set model [load_charts_model -csv data/one_cycle.csv -comment_header]

set plot [create_charts_plot -model $model -type force_directed -columns {{from 0} {to 1}}]

set_charts_property -plot $plot -name node.scaled -value 0
set_charts_property -plot $plot -name node.text.scaled -value 1
set_charts_property -plot $plot -name node.size -value 0.5P
set_charts_property -plot $plot -name node.shapeType -value CIRCLE

set_charts_property -plot $plot -name edge.scaled -value 0
set_charts_property -plot $plot -name edge.arrow  -value 1
set_charts_property -plot $plot -name edge.width -value 0.1P
set_charts_property -plot $plot -name edge.shapeType -value ARC
