set model [load_charts_model -csv data/xy_100000.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}} -title "Scatter (100000 points)"]
