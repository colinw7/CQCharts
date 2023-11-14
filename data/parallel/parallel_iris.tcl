set model [load_charts_model -tsv data/iris.tsv -first_line_header]

set plot [create_charts_plot -model $model -type parallel \
  -columns {{x species} {color species} {y {petalLength petalWidth sepalLength sepalWidth}}}]

set_charts_property -plot $plot -name coloring.defaultPalette -value viridis

set_charts_property -plot $plot -name lines.stroke.width -value 1.5px
set_charts_property -plot $plot -name points.symbol.size -value 12.5px

set_charts_property -plot $plot -name axis.local -value 0
set_charts_property -plot $plot -name axis.grid.visible -value MAJOR
