set symbol_data [list \
 [list 1 10 one   fred] \
 [list 2 20 two   bill] \
 [list 3 30 three harry] \
 [list 4 20 three one] \
 [list 5 15 four  two] \
 [list 6 25 two   three] \
 [list 7 35 two   two] \
 [list 8 20 one   harry] \
 [list 9 5  one   bill] \
]

set model [load_charts_model -var symbol_data -transpose]

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolSize 2}} \
  -title "Scatter Symbol Size"]

set_charts_property -plot $plot1 -name mapping.symbolSize.enabled -value 1

set_charts_property -plot $plot1 -name mapping.symbolSize.size_map \
  -value {{one 0.1P} {two 0.2P} {three 0.4P} {four 0.8P}}

set_charts_property -plot $plot1 -name margins.inner.left   -value 20px
set_charts_property -plot $plot1 -name margins.inner.right  -value 20px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 20px
set_charts_property -plot $plot1 -name margins.inner.top    -value 20px

set_charts_property -plot $plot1 -name points.size -value 12px
