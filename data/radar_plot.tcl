proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/radar.csv -first_line_header]

set plot [create_plot -type radar -columns "name=0,value=1 2 3 4 5" -title "Radar Plot"]

connect_chart -plot $plot -from objIdPressed -to objPressed
