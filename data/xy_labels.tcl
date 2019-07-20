proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/lines.csv -comment_header \
  -column_type {{{0 time} {format %Y%m%d} {oformat %b}}}]

set plot [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {label 2} {color 3} {symbolType 4}} \
  -title "XY Labels"]

set_charts_property -plot $plot -name yaxis.grid.lines           -value MAJOR
set_charts_property -plot $plot -name points.visible             -value 1
set_charts_property -plot $plot -name points.symbol.type         -value circle
set_charts_property -plot $plot -name points.symbol.size         -value 12px
set_charts_property -plot $plot -name points.symbol.fill.visible -value 1
