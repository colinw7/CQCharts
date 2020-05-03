set font_data [list \
 [list 1 10 One   {15 bold}] \
 [list 2 20 Two   {20 italic}] \
 [list 3 30 Three {25 normal}] \
]

set model [load_charts_model -var font_data -transpose]

set_charts_data -model $model -column 3 -name column_type -value {font}

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {name 2} {font 3}} \
  -parameter {pointLabels 1}]

set_charts_property -plot $plot -name margins.inner.left   -value 50px
set_charts_property -plot $plot -name margins.inner.right  -value 50px
set_charts_property -plot $plot -name margins.inner.bottom -value 50px
set_charts_property -plot $plot -name margins.inner.top    -value 50px
