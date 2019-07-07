set symbol_data [list \
 [list 1 10 15] \
 [list 2 20 10] \
 [list 3 30 5] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 2 -name column_type -value {{symbolSize:mapped 1}}

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolSize 2}}]

set_charts_property -plot $plot -name margins.inner.left   -value 50px
set_charts_property -plot $plot -name margins.inner.right  -value 50px
set_charts_property -plot $plot -name margins.inner.bottom -value 50px
set_charts_property -plot $plot -name margins.inner.top    -value 50px

set_charts_property -plot $plot -name mapping.symbol_size.enabled -value 1
set_charts_property -plot $plot -name mapping.symbol_size.min     -value "5"
set_charts_property -plot $plot -name mapping.symbol_size.max     -value "40"