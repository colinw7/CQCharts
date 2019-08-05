set symbol_data [list \
 [list 1 10 15] \
 [list 2 20 12] \
 [list 3 30 8] \
 [list 4 20 3] \
 [list 5 15 5] \
 [list 6 25 11] \
 [list 7 35 9] \
 [list 8 20 7] \
 [list 9 5  14] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 2 -name column_type -value {{symbolSize:mapped 1}}

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolSize 2}} \
  -title "Scatter Symbol Sizes"]

set_charts_property -plot $plot1 -name mapping.symbolSize.enabled -value 1
set_charts_property -plot $plot1 -name mapping.symbolSize.min     -value 5
set_charts_property -plot $plot1 -name mapping.symbolSize.max     -value 40

set_charts_property -plot $plot1 -name margins.inner.left   -value 50px
set_charts_property -plot $plot1 -name margins.inner.right  -value 50px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 50px
set_charts_property -plot $plot1 -name margins.inner.top    -value 50px

set plot2 [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {symbolSize 2}} \
  -title "XY Symbol Sizes"]

set_charts_property -plot $plot2 -name mapping.symbolSize.enabled -value 1
set_charts_property -plot $plot2 -name mapping.symbolSize.min     -value 5
set_charts_property -plot $plot2 -name mapping.symbolSize.max     -value 40

set_charts_property -plot $plot2 -name coloring.type              -value INDEX
set_charts_property -plot $plot2 -name points.visible             -value 1
set_charts_property -plot $plot2 -name points.symbol.type         -value circle
set_charts_property -plot $plot2 -name points.symbol.size         -value 12px
set_charts_property -plot $plot2 -name points.symbol.fill.visible -value 1
set_charts_property -plot $plot2 -name points.symbol.fill.color   -value palette
set_charts_property -plot $plot2 -name yaxis.grid.lines           -value MAJOR

set_charts_property -plot $plot2 -name margins.inner.left   -value 50px
set_charts_property -plot $plot2 -name margins.inner.right  -value 50px
set_charts_property -plot $plot2 -name margins.inner.bottom -value 50px
set_charts_property -plot $plot2 -name margins.inner.top    -value 50px

#---

place_charts_plots -horizontal $plot1 $plot2
