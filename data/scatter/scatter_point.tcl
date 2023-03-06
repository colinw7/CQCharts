set model [load_charts_model -csv data/geometry.csv -first_line_header \
  -column_type {string real point rect}]

set plot [create_charts_plot -type scatter -columns {{x 2[x]} {y 2[y]}}]
