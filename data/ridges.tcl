proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/lincoln-weather.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{time} {format %Y-%m-%d}}

set plot [create_charts_plot -model $model -type distribution \
  -columns [list [list group "CST\[%B\]"] [list value 2]] -title "Temperature Ridge Lines"]

set_charts_property -plot $plot -name density.enabled -value 1

#set plot [create_charts_plot -model $model -type distribution \
# -columns [list [list group "CST\[%B\]"] [list value "Mean Temperature \[F\]"]]]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
