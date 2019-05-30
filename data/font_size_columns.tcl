set font_data [list \
 [list 1 10 One   15] \
 [list 2 20 Two   10] \
 [list 3 30 Three  5] \
]

set model [load_charts_model -var font_data -transpose]

# map font directly in model
#set_charts_data -model $model -column 2 -name column_type -value {{fontSize:mapped 1}}

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {name 2} {fontSize 3}} \
  -parameter "textLabels=1"]

set_charts_property -plot $plot -name margin.inner.left   -value 50px
set_charts_property -plot $plot -name margin.inner.right  -value 50px
set_charts_property -plot $plot -name margin.inner.bottom -value 50px
set_charts_property -plot $plot -name margin.inner.top    -value 50px

set_charts_property -plot $plot -name font.map.size.enabled -value 1
set_charts_property -plot $plot -name font.map.size.min     -value "5"
set_charts_property -plot $plot -name font.map.size.max     -value "40"
