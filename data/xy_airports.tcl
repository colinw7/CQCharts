proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/airports.csv -comment_header]

set plot [create_plot -model $model -type xy \
  -columns "x=6,y=5,name=1,id=0,tip=1" \
  -properties "lines.visible=0,points.visible=1" \
  -title "airports"]

connect_charts -plot $plot -from objIdPressed -to objPressed
