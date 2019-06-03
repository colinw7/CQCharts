proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

define_charts_proc color_value { arg } {
  return [expr {$arg < 0.0 ? "#aa4444" : "#44aa44"}]
}

set model [load_charts_model -csv data/gaussian.txt -comment_header]

process_charts_model -model $model -add -expr "color_value(column(0))" -header "color" -type color

set plot [create_charts_plot -model $model -type distribution \
  -columns {{value 0} {color 1}} \
  -properties {{mapping.color.enabled 0}} \
  -properties {{dataLabel.visible 1} {dataLabel.position TOP_OUTSIDE}} \
  -title "color split distribution"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
