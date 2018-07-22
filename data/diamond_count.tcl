proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/diamonds.csv -first_line_header]

set view [create_view]

#set plot1 [create_plot -view $view -model $model -type distribution -columns "value=cut"]
#connect_chart -plot $plot1 -from objIdPressed -to objPressed

#set plot2 [create_plot -view $view -model $model -type pie -columns "group=cut"]
#set_property -plot $plot2 -name options.count -value 1
#connect_chart -plot $plot2 -from objIdPressed -to objPressed

set plot3 [create_plot -view $view -model $model -type distribution -columns "value=cut"]
connect_chart -plot $plot3 -from objIdPressed -to objPressed

#place_plots -horizontal $plot1 $plot2
