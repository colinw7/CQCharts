set model [load_charts_model -csv data/geometry.csv -first_line_header \
  -column_type {polygon}]

set plot [create_charts_plot -type scatter -columns {{x Polygon[x0]} {y Polygon[y0]}}]
