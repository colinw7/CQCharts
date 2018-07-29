proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/diamonds.csv -first_line_header]

filter_model -model $model -expr {$color == "J" || $color == "D"}
#filter_model -model $model -expr {@{color} == "J" || @{color} == "D"}

set view [create_view]

set plot [create_plot -view $view -model $model -type distribution -columns "value=cut,group=color"]
#set plot [create_plot -view $view -model $model -type distribution -columns "value=cut,group=color" -where {$color == "J" || $color == "D"}]

connect_chart -plot $plot -from objIdPressed -to objPressed
