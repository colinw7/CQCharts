proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/airports.csv -comment_header]

# iata,name,city,state,country,latitude,longitude
set plot [create_charts_plot -model $model -type xy \
  -columns {{x 6} {y 5} {label 1} {id 0} {tips {2 3 4}}} \
  -properties {{lines.visible 0} {points.visible 1}} \
  -title "airports"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
