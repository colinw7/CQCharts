proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

set modelId [load_charts_model -csv data/p2.csv]

set plotId [create_charts_plot -type scatter -columns {{x 0} {y 1}}]

set viewId [get_charts_property -plot $plotId -name viewId]

set button [qt_create_widget -type QPushButton -name "button"]

qt_set_property -object $button -property text -value "Button"

set widget1 [create_charts_widget_annotation -plot $plotId -id one \
  -position {0.5 0.5} -widget $button]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
