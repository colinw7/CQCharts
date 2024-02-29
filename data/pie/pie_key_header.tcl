set model [load_charts_model -csv data/ages.csv -first_line_header \
  -column_type {{{1 integer}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {values 1}} -title "pie chart"]

set_charts_property -plot $plot -name key.orientation        -value Horizontal
set_charts_property -plot $plot -name key.header.inline      -value 1
set_charts_property -plot $plot -name key.header.text.string -value Header
