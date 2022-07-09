proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model1 [load_charts_model -csv data/world.csv -comment_header \
  -column_type {{{1 polygon_list}}}]

set plot1 [create_charts_plot -model $model1 -type geometry \
  -columns {{name 0} {geometry 1}} \
  -title "geometry"]

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed
