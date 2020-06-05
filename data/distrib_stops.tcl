set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type distribution \
  -columns {{value 0}}]

set_charts_property -plot $plot -name coloring.defaultPalette -value {red_green}
set_charts_property -plot $plot -name coloring.type -value {X_VALUE}
set_charts_property -plot $plot -name coloring.xStops -value {0}
