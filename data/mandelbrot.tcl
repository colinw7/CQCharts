proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -csv data/mandelbrot.csv]

set plot [create_plot -model $model -type image]

connect_charts -plot $plot -from objIdPressed -to objPressed
