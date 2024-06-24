set model [create_charts_random_model -rows 10 -columns 2 -ranges {{0.4 0.6} {0 1.0}}]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}} -title "Scatter (Random)" \
  -xmin 0 -ymin 0 -xmax 1 -ymax 1]

set view [get_charts_data -plot $plot -name view]
