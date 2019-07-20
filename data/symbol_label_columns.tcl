proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/lines.csv -comment_header \
  -column_type {{{0 time} {format %Y%m%d} {oformat %b}}}]

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {label 2} {color 3} {symbolType 4} {fontSize 5}} \
  -title "Scatter Labels"]

set_charts_property -plot $plot1 -name mapping.color.enabled      -value 0
set_charts_property -plot $plot1 -name mapping.symbolType.enabled -value 0
set_charts_property -plot $plot1 -name mapping.fontSize.enabled   -value 0

set_charts_property -plot $plot1 -name labels.visible      -value 1
set_charts_property -plot $plot1 -name labels.position     -value TOP_OUTSIDE
set_charts_property -plot $plot1 -name symbol.type         -value circle
set_charts_property -plot $plot1 -name symbol.size         -value 12px
set_charts_property -plot $plot1 -name symbol.fill.visible -value 1
set_charts_property -plot $plot1 -name yaxis.grid.lines    -value MAJOR

set_charts_property -plot $plot1 -name margins.inner.left   -value 50px
set_charts_property -plot $plot1 -name margins.inner.right  -value 50px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 50px
set_charts_property -plot $plot1 -name margins.inner.top    -value 50px

#---

set plot2 [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {label 2} {color 3} {symbolType 4} {fontSize 5}} \
  -title "XY Labels"]

set_charts_property -plot $plot2 -name coloring.type              -value INDEX
set_charts_property -plot $plot2 -name labels.visible             -value 1
set_charts_property -plot $plot2 -name labels.position            -value TOP_OUTSIDE
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
