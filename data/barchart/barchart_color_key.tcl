set model [load_charts_model -csv data/mtcars.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{name model} {values mpg} {group cyl} {color gear}}]

set_charts_property -plot $plot -name key.colorColumn -value 1
set_charts_property -plot $plot -name key.colorColumn -value 1
