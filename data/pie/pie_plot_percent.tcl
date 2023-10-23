set model [load_charts_model -csv data/ages.csv -first_line_header \
  -column_type {{{1 integer}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {values 1}} -title "pie chart"]

set_charts_property -plot $plot -name labels.radius -value 0.5

set_charts_property -plot $plot -name options.showLabel    -value 0
set_charts_property -plot $plot -name options.showValue    -value 1
set_charts_property -plot $plot -name options.valuePercent -value 1
