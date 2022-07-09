set model [load_charts_model -csv data/geom_boxes_1.csv -first_line_header \
  -column_type {{{1 rectangle}}}]

set plot [create_charts_plot -model $model -type geometry \
  -columns {{name 0} {geometry 1}} -xmin 0 -ymin 0 -xmax 100 -ymax 100]
