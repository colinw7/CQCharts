#set model [load_charts_model -csv data/force_directed_color.csv -first_line_header]

#set plot [create_charts_plot -model $model -type forcedirected \
#  -columns {{from 0} {to 1} {value 2} {attributes 3}}]

set model [load_charts_model -csv data/connections3.csv -first_line_header]

set plot [create_charts_plot -model $model -type forcedirected \
  -columns {{from 0} {to 1} {value 2} {group 3} {attributes 4}}]

set_charts_property -plot $plot -name animation.minDelta -value -1.0
set_charts_property -plot $plot -name animation.maxSteps -value -1

set_charts_property -plot $plot -name coloring.defaultPalette -value moreland

set_charts_property -plot $plot -name edge.arrow -value {true}
set_charts_property -plot $plot -name edge.scaled -value {false}
set_charts_property -plot $plot -name edge.shapeType -value {ARC}
set_charts_property -plot $plot -name edge.valueColored -value {true}

set_charts_property -plot $plot -name node.scaled -value {false}
set_charts_property -plot $plot -name node.valueColored -value {true}

set_charts_property -plot $plot -name node.tipNameLabel  -value {My Name}
set_charts_property -plot $plot -name node.tipValueLabel -value {My Value}
