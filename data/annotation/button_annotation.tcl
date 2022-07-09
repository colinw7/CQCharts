proc viewAnnotationSlot { viewId id } {
  puts "viewAnnotationSlot: $viewId, $id"

  set text [get_charts_property -annotation $id -name text.string]
  puts "text: $text"
}

proc plotAnnotationSlot { viewId plotId id } {
  puts "plotAnnotationSlot: $viewId, $plotId, $id"

  set text [get_charts_property -annotation $id -name text.string]
  puts "text: $text"
}

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set view [get_charts_property -plot $plot -name viewId]

set button1 [create_charts_button_annotation -view $view -id one -position {25 25 V} -text "One"]
set button2 [create_charts_button_annotation -plot $plot -id two -position {75 75 P} -text "Two"]

connect_charts_signal -view $view -from annotationIdPressed -to viewAnnotationSlot
connect_charts_signal -plot $plot -from annotationIdPressed -to plotAnnotationSlot
