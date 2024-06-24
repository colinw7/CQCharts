set model [create_charts_fireworks_model]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {color 2}} -title "Scatter (Fireworks)" \
  -xmin 0 -ymin 0 -xmax 100 -ymax 200]

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name background.fill.color -value black

set_charts_property -plot $plot -name plotBox.fill.color -value black
set_charts_property -plot $plot -name dataBox.fill.color -value black

set_charts_property -plot $plot -name key.visible -value 0
set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
