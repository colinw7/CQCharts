set symbol_data [list \
 [list 1 10 circle fred] \
 [list 2 20 triangle bill] \
 [list 3 30 square harry] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 3 -name column_type \
#  -value {{symbol:mapped 1} {palette plasma}}

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolType 3}}]

set_charts_property -plot $plot -name margins.inner.left   -value 20px
set_charts_property -plot $plot -name margins.inner.right  -value 20px
set_charts_property -plot $plot -name margins.inner.bottom -value 20px
set_charts_property -plot $plot -name margins.inner.top    -value 20px

#set_charts_property -plot $plot -name mapping.symbol_type.enabled -value 1
#set_charts_property -plot $plot -name mapping.symbol.type.symbols -value "triangle,circle,square"
