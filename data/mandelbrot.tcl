proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/mandelbrot.csv]

set view [create_charts_view]

set_charts_property -view $view -name options.antiAlias -value false

set plot [create_charts_plot -view $view -model $model -type image]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
