proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/multi_bar.csv -first_line_header \
  -column_type {{{1 integer}}}]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{group 0} {value {1 2 3 4 5 6 7}}} \
  -title "multiple bar chart"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
