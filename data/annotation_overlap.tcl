set model [load_charts_model -csv data/geom_boxes.csv -first_line_header \
  -column_type {{{1 rectangle}}}]

set plot [create_charts_plot -type geometry \
  -columns {{name 0} {geometry 1}} \
  -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set ann [create_charts_point_annotation -plot $plot -position {40 40}]
set ann [create_charts_point_annotation -plot $plot -position {50 50}]
set ann [create_charts_point_annotation -plot $plot -position {60 60}]
