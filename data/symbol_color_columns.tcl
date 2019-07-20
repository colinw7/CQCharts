set symbol_data [list \
 [list 1 10 circle    red] \
 [list 2 20 triangle  green] \
 [list 3 30 square    blue] \
 [list 4 20 star5     orange] \
 [list 5 15 star6     cyan] \
 [list 6 25 pentagon  yellow] \
 [list 7 35 itriangle black] \
 [list 8 20 hline     purple] \
 [list 9 5  vline     magenta] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 3 -name column_type \
#  -value {{symbol:mapped 1} {palette plasma}}

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {color 3}}]

set_charts_property -plot $plot1 -name mapping.color.enabled -value 0

set_charts_property -plot $plot1 -name margins.inner.left   -value 20px
set_charts_property -plot $plot1 -name margins.inner.right  -value 20px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 20px
set_charts_property -plot $plot1 -name margins.inner.top    -value 20px

set_charts_property -plot $plot1 -name symbol.size -value 12px

set plot2 [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {color 3}}]

set_charts_property -plot $plot2 -name points.visible -value 1

set_charts_property -plot $plot2 -name margins.inner.left   -value 50px
set_charts_property -plot $plot2 -name margins.inner.right  -value 50px
set_charts_property -plot $plot2 -name margins.inner.bottom -value 50px
set_charts_property -plot $plot2 -name margins.inner.top    -value 50px

set_charts_property -plot $plot2 -name points.visible             -value 1
set_charts_property -plot $plot2 -name points.symbol.size         -value 12px
set_charts_property -plot $plot2 -name points.symbol.fill.visible -value 1

#set_charts_property -plot $plot2 -name mapping.color.enabled -value 0

#---

place_charts_plots -horizontal $plot1 $plot2
