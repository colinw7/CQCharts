proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type distribution \
 -columns {{values cut} {group color}}]

set_charts_property -plot $plot -name scatter.visible -value 1
set_charts_property -plot $plot -name scatter.factor  -value 0.25

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
