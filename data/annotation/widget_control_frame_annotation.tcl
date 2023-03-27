proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

# link column model
set model [load_charts_model -csv data/chord-cities.csv -comment_header]

set plot [create_charts_plot -model $model -type chord \
  -columns {{link 0} {value 1} {group 2} {color 3} {controls 2}} -title "chord"]

#---

proc set_text_property { type w text } {
  if {[qt_has_property -object $w -property text -writable]} {
    qt_set_property -object $w -property text -value $text
  }
}

set control [qt_create_widget -type CQCharts:PlotControlFrame -name control]

set ann [create_charts_widget_annotation -plot $plot -id control \
  -position [list -1 -1] -widget $control]

set control [get_charts_property -annotation $ann -name widget]

qt_activate_slot -name $control -slot "setPlotControls()"

connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot
