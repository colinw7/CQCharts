set symbol_data [list \
 [list 1  10 circle    fred   alpha  ] \
 [list 2  20 triangle  bill   beta   ] \
 [list 3  30 box       harry  gamma  ] \
 [list 4  20 star5     one    delta  ] \
 [list 5  15 star6     two    epsilon] \
 [list 6  25 pentagon  three  delta  ] \
 [list 7  35 itriangle four   gamma  ] \
 [list 8  20 pentagon  five   beta   ] \
 [list 9  5  ipentagon six    alpha  ] \
 [list 10 10 hexagon   seven  epsilon] \
 [list 11 15 octagon   eight  delta  ] \
 [list 12 20 paw       nine   beta   ] \
 [list 13 15 hash      ten    alpha  ] \
 [list 14 25 hline     eleven gamma  ] \
 [list 15 10 vline     twelve delta  ] \
]

set model [load_charts_model -var symbol_data -transpose]

# map symbol directly in model
#set_charts_data -model $model -column 3 -name column_type \
#  -value {{symbolType:mapped 1} {palette plasma}}

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolType 2} {color 3} {symbolSize 4}} \
  -title "Scatter Symbol Type"]

set_charts_property -plot $plot1 -name mapping.symbolType.enabled -value 0
#set_charts_property -plot $plot1 -name mapping.symbolType.symbols -value "triangle,circle,box"

set_charts_property -plot $plot1 -name margins.inner.left   -value 20px
set_charts_property -plot $plot1 -name margins.inner.right  -value 20px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 20px
set_charts_property -plot $plot1 -name margins.inner.top    -value 20px

set_charts_property -plot $plot1 -name points.size -value 12px
