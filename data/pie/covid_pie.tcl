set palette [create_charts_palette -palette state_colors]

set_charts_palette -palette state_colors -name defined_colors \
 -value [list {0 #3d5b77} {1 #7591e0} {2 #d64e31} {3 #5fcc82}]

set model [load_charts_model -csv data/covid_states.csv -first_line_header]

sort_charts_model -model $model -column {Age 18-64}

set plot [create_charts_plot -model $model -type pie \
  -columns {{label State} {values {{State}}} {radius {Age 18-64}} {color Region}} \
  -title "% of adults with at least one COVID-19 shot"]

set_charts_property -plot $plot -name title.visible -value 0
set_charts_property -plot $plot -name key.visible -value 0

set_charts_property -plot $plot -name dataBox.fill.visible -value 0

set_charts_property -plot $plot -name coloring.defaultPalette -value state_colors

set_charts_property -plot $plot -name labels.rotated -value 1
set_charts_property -plot $plot -name labels.text.font -value -4

set_charts_property -plot $plot -name radius.text.font -value -4

set_charts_property -plot $plot -name options.donut -value 1
set_charts_property -plot $plot -name options.gapAngle -value 2
