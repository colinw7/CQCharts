proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/diamonds.csv -first_line_header]

filter_charts_model -model $model -expr {$color == "J" || $color == "D"}
#filter_charts_model -model $model -expr {@{color} == "J" || @{color} == "D"}

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type distribution \
  -columns {{value cut} {group color}}]
#set plot [create_charts_plot -view $view -model $model -type distribution \
# -columns {{value cut} {group color}} \
# -where {$color == "J" || $color == "D"}]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
