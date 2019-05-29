proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/lines.csv -comment_header \
  -column_type "0#time:format=%Y%m%d,oformat=%b"]

set plot [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y 1} {pointLabel 2} {pointColor 3} {pointSymbol 4}} \
  -properties {{points.symbol.type circle} {points.symbol.fill.visiblei 1}} \
  -properties {{yaxis.grid.line.major.visible 1}} \
  -title "Line Chart"]
