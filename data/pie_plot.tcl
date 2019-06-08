proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/ages.csv -first_line_header \
  -column_type {{{1 integer}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {value 1}} -title "pie chart"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
