set model [load_charts_model -csv data/mandelbrot.csv]

set view [create_charts_view]

set_charts_property -view $view -name options.antiAlias -value false

set plot [create_charts_plot -view $view -model $model -type scatter3d \
 -columns {{x @ROW} {y @COLUMN} {z @CELL}}]
