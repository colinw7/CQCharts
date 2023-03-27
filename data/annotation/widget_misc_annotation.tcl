proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

# link column model
set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

#---

set w1 [qt_create_widget -type CQCharts:AlphaEdit -name w1]
set w2 [qt_create_widget -type CQCharts:AngleEdit -name w2]

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

qt_add_child_widget -parent $frame -child $w1
qt_add_child_widget -parent $frame -child $w2

set ann [create_charts_widget_annotation -plot $plot -id ann \
  -position [list 50 50] -widget $frame]

connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot
