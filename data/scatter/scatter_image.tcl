# x,y,image
set model [load_charts_model -csv data/scatter_image.csv -first_line_header \
  -column_type {{{2 image}}}]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {image 2}}]

set_charts_property -plot $plot -name points.size -value 0.1P
