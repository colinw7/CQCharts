set model [load_charts_model -csv data/connections_meta.csv -first_line_header]

set plot [create_charts_plot -model $model -type forcedirected \
  -columns {{from 0} {to 1} {value 2} {group 3}}]

set_charts_property -plot $plot -name coloring.defaultPalette -value moreland

set_charts_property -plot $plot -name edge.arrow -value {true}
set_charts_property -plot $plot -name edge.scaled -value {false}
set_charts_property -plot $plot -name edge.shapeType -value {ARC}
set_charts_property -plot $plot -name edge.valueColored -value {true}

set_charts_property -plot $plot -name node.scaled -value {false}
set_charts_property -plot $plot -name node.valueColored -value {true}

set_charts_property -plot $plot -name node.tipNameLabel  -value {My Name}
set_charts_property -plot $plot -name node.tipValueLabel -value {My Value}
