proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/ages.csv -first_line_header -column_type "1#integer"]

set view [create_view]

set plot [create_plot -view $view -model $model -type pie -columns "label=0,value=1" -title "pie chart"]

connect_chart -plot $plot -from objIdPressed -to objPressed
