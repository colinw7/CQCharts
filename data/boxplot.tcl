proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set plot [create_charts_plot -type box -columns {{group 0} {value 2}} -title "boxplot" \
 -properties {{options.horizontal 0}}]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
