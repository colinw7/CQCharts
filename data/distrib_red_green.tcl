proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

add_process_model_proc color_value { arg } {
  return [expr {$arg < 0.0 ? "red" : "green"}]
}

set model [load_model -csv data/gaussian.txt -comment_header]

process_model -model $model -add -expr "color_value(column(0))" -header "color"

set plot [create_plot -model $model -type distribution \
  -columns "value=0,color=1" \
  -properties "color.map.enable=0" \
  -properties "dataLabel.visible=1,dataLabel.position=TOP_OUTSIDE" \
  -title "color split distribution"]

connect_chart -plot $plot -from objIdPressed -to objPressed
