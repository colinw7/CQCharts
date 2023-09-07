set model [load_charts_model -csv data/scatter_100.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}} \
  -title "Scatter Color Stops"]

set view [get_charts_data -plot $plot -name view]

set_charts_property -plot $plot -name coloring.xStops -value {{10 0.0} {20 0.8} {20 0.8} {100 1.0}}
set_charts_property -plot $plot -name coloring.type -value X_VALUE
set_charts_property -view $view -name coloring.defaultPalette -value reds
