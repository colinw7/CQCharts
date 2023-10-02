set model [load_charts_model -csv data/distrib_zero.csv -comment_header]

set plot1 [create_charts_plot -model $model -type distribution \
  -columns {{values 0}}]
set plot2 [create_charts_plot -model $model -type distribution \
  -columns {{values 1}}]
set plot3 [create_charts_plot -model $model -type distribution \
  -columns {{values 2}}]

set view [get_charts_data -plot $plot1 -name view]

place_charts_plots -view $view -rows 2 [list $plot1 $plot2 $plot3]

set_charts_property -view $view -name state.zoomMode -value VIEW
set_charts_property -view $view -name state.zoomScroll -value 1
