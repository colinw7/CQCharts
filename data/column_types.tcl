set model [load_charts_model -csv data/name_values.csv -first_line_header \
  -column_type {{double} {double} {name_values}}]

set plot [create_charts_plot -type scatter \
  -columns {{x 0} {y 1} {color 2[color]} {symbolSize 2[width]}}]
