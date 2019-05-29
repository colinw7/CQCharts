proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {value {1 2 3 4 5 6 7}}} -title "multi column pie chart with row grouping"]

set_charts_property -plot $plot -name "dataGrouping.rowGroups" -value 1

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
