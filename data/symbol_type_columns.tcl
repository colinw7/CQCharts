set symbol_data [list \
 [list 1 10 circle    fred] \
 [list 2 20 triangle  bill] \
 [list 3 30 square    harry] \
 [list 4 20 star5     one] \
 [list 5 15 star6     two] \
 [list 6 25 pentagon  three] \
 [list 7 35 itriangle four] \
 [list 8 20 hline     five] \
 [list 9 5  vline     six] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 3 -name column_type \
#  -value {{symbolType:mapped 1} {palette plasma}}

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolType 2}} \
  -title "Scatter Symbol Type"]

set_charts_property -plot $plot1 -name mapping.symbolType.enabled -value 0
#set_charts_property -plot $plot1 -name mapping.symbolType.symbols -value "triangle,circle,square"

set_charts_property -plot $plot1 -name margins.inner.left   -value 20px
set_charts_property -plot $plot1 -name margins.inner.right  -value 20px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 20px
set_charts_property -plot $plot1 -name margins.inner.top    -value 20px

set_charts_property -plot $plot1 -name symbol.size -value 12px

set plot2 [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {symbolType 2}} \
  -title "XY Symbol Type"]

#set_charts_property -plot $plot2 -name mapping.symbolType.enabled -value 0
#set_charts_property -plot $plot2 -name mapping.symbolType.symbols -value "triangle,circle,square"

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
