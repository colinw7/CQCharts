set model [load_charts_model -csv data/3d.csv -first_line_header]

set view [create_charts_view -3d]

set plot [create_charts_plot -view $view -model $model -type scatter3d \
  -columns {{x 1} {y 3} {z 2} {color 3}} -title "Scatter"]
