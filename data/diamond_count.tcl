proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set view [create_charts_view]

set plot1 [create_charts_plot -view $view -model $model -type distribution -columns {{value cut}}]
connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

set plot2 [create_charts_plot -view $view -model $model -type pie -columns {{group cut}}]
set_charts_property -plot $plot2 -name options.count -value 1
connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed

set plot3 [create_charts_plot -view $view -model $model -type distribution -columns {{value cut}}]
connect_charts_signal -plot $plot3 -from objIdPressed -to objPressed

place_charts_plots -horizontal $plot1 $plot2 $plot3
