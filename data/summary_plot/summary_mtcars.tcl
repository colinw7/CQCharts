set model [load_charts_model -csv data/mtcars.csv -first_line_header]
set plot [create_charts_plot -type summary -model $model \
  -columns {{columns {1 2 3 4 5 6 7 8 9 10 11}}}]

set_charts_property -plot $plot -name options.selectMode -value DATA

set_charts_property -plot $plot -name xaxis.visible -value 1
set_charts_property -plot $plot -name yaxis.visible -value 1
