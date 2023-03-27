proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x sepalLength} {y sepalWidth}} \
  -properties {{symbol.symbol circle} {symbol.size 5px}} \
  -properties {{xaxis.userLabel {Sepal Length}} {yaxis.userLabel {Sepal Width}}}]

set propertyEditGroup [qt_create_widget -type CQCharts:PlotPropertyEditGroup -name edit]

qt_activate_slot -name $propertyEditGroup -slot "addSlot(QString)" -args font
qt_activate_slot -name $propertyEditGroup -slot "addSlot(QString)" -args xColumn
qt_activate_slot -name $propertyEditGroup -slot "addSlot(QString)" -args yColumn
qt_activate_slot -name $propertyEditGroup -slot "addSlot(QString)" -args showBoxes
 
set ann [create_charts_widget_annotation -plot $plot -id modelView \
  -position [list 80 80 V] -widget $propertyEditGroup]

#set_charts_property -annotation $ann -name interactive -value 1
#set_charts_property -annotation $ann -name rectangle -value {80 80 100 100 V}
