proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

proc set_text_property { type w text } {
  if {[qt_has_property -object $w -property text -writable]} {
    qt_set_property -object $w -property text -value $text
  }
}

set button1 [qt_create_widget -type QPushButton -name button1]
set button2 [qt_create_widget -type QPushButton -name button2]
set button3 [qt_create_widget -type QPushButton -name button3]

set_text_property QPushButton $button1 {Button 1}
set_text_property QPushButton $button2 {Button 2}
set_text_property QPushButton $button3 {Button 3}

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

qt_add_child_widget -parent $frame -child $button1
qt_add_child_widget -parent $frame -child $button2
qt_add_child_widget -parent $frame -child $button3

set ann [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 0 0] -widget $frame]

connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot
