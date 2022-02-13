set model [load_charts_model -csv data/scatter_100.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {color 1}} \
  -title "Scatter Color Stops"]

set_charts_property -plot $plot -name coloring.xStops -value "10 50 100"
