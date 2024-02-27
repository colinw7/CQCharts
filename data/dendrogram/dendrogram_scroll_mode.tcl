set model [load_charts_model -csv data/periodic.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{value AtomicMass} {name Element} {group Type}} -title "Periodic"]

set_charts_property -plot $::plot -name options.calcNodeTextSize -value 1
set_charts_property -plot $::plot -name options.pixelScaled      -value 1

set view [get_charts_data -plot $plot -name view]

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QHBoxLayout -parent $frame]

proc scrollClick { } {
  set_charts_property -plot $::plot -name options.fitMode -value SCROLL
  set_charts_property -plot $::plot -name options.spreadNodeOverlaps -value 1
  set_charts_data -view $::view -name fit -value 1
}

proc zoomClick { } {
  set_charts_property -plot $::plot -name options.fitMode -value SCALE
  set_charts_property -plot $::plot -name options.spreadNodeOverlaps -value 0
  set_charts_data -view $::view -name fit -value 1
}

set scrollButton [qt_create_widget -type QPushButton -name scrollButton]
set zoomButton   [qt_create_widget -type QPushButton -name zoomButton]
  
qt_set_property -object $scrollButton -property text -value Scroll
qt_set_property -object $zoomButton   -property text -value Zoom

qt_connect_widget -name $scrollButton -signal "clicked()" -proc scrollClick
qt_connect_widget -name $zoomButton   -signal "clicked()" -proc zoomClick

qt_add_child_widget -parent $frame -child $scrollButton
qt_add_child_widget -parent $frame -child $zoomButton

set annotation [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 80 97 V] -widget $frame]

proc expandAll { view plot } {
  if {! [get_charts_property -plot $::plot -name state.allExpanded]} {
    execute_charts_slot -plot $plot -name expand_all
  }
}

connect_charts_signal -plot $plot -from plotObjsAdded -to expandAll
