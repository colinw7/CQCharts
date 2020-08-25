proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

set propertyEdit [qt_create_widget -type CQChartsPlotPropertyEdit -name edit]

qt_set_property -object $propertyEdit -property propertyName -value showBoxes
 
set ann [create_charts_widget_annotation -plot $plot -id modelView \
  -position [list -1 -1] -widget $propertyEdit]

#set_charts_property -annotation $ann -name interactive -value 1
set_charts_property -annotation $ann -name rectangle -value {0 0 1 1}
