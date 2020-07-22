proc objPressed { view plot id } {
  #echo "$view $plot $id"

  set model [get_charts_data -plot $plot -name model]

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "Inds"

  foreach ind $inds {
    echo "$ind"
  }
}

set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type bubble \
  -columns {{group 0} {value @GROUP} {id 0}} \
  -title "distribution"]

set_charts_property -plot $plot -name dataGrouping.bucket.auto -value 1
set_charts_property -plot $plot -name filter.minSize           -value 100

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
