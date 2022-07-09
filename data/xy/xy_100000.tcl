set modelId [load_charts_model -csv data/xy_100000.csv -first_line_header]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY (100000 Points)" \
  -properties {{lines.visible 0} {points.visible 1}}]

set_charts_property -plot $plot1 -name lines.visible -value 0
