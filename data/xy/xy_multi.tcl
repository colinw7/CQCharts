proc objPressed { view plot id } {
  echo "objPressed: $view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "  $inds"
}

proc selectionChanged { view plot } {
  echo "selectionChanged: $view $plot"

  set objs [get_charts_data -plot $plot -name selected_objects]

  foreach obj $objs {
    set inds [get_charts_data -plot $plot -object $obj -name inds]

    echo "  $obj : $inds"
  }
}

set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set plot [create_charts_plot -model $model -type xy \
  -columns {{x 0} {y {1 2 3}}} \
  -properties {{lines.visible 1} {points.visible 0} {fillUnder.visible 1}} \
  -title "multiple xy plot"]

connect_charts_signal -plot $plot -from objIdPressed     -to objPressed
connect_charts_signal -plot $plot -from selectionChanged -to selectionChanged
