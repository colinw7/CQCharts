proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/diamonds.csv -first_line_header]

set view [create_view]

set plot [create_plot -view $view -model $model -type scatter -columns "x=carat,y=price"]

set_property -plot $plot -name grid.enabled -value 1

connect_chart -plot $plot -from objIdPressed -to objPressed
