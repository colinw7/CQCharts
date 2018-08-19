proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_property -annotation $id -name position]
  puts $pos

  set text [get_property -annotation $id -name text]
  puts $text
}

set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "This plot has a very long title which we might want to format on multiple lines"]

set viewId [get_property -plot $plotId -name viewId]

set textAnnotationId1 [create_text_shape -plot $plotId -id one -position {0 -60} -text "One"]

set textAnnotationId2 [create_text_shape -plot $plotId -id two -position {0 -50} -text "Two" -color red]

set textAnnotationId3 [create_text_shape -plot $plotId -id three -position {0 -40} -text "Three" -color green -alpha 0.5]

set textAnnotationId4 [create_text_shape -plot $plotId -id four -position {0 -30} -text "Four"  -color blue  -angle 45]

set textAnnotationId5 [create_text_shape -plot $plotId -id long -position {0 0} -text "The rain in spain falls mainly on the plain. She sell sea shells on the sea shore. The quick brown fox jumps over the lazy dog." -border 1 -background 1 -corner_size 12]

set textAnnotationId6 [create_text_shape -plot $plotId -id long -position {100 100 pixel} -text "<table><tr><td>One</td><td>Two</td></tr><tr><td>Three</td><td>Four</td></tr></table>" -border 1 -background 1 -corner_size 12 -html]

connect_chart -plot $plotId -from annotationIdPressed -to annotationSlot
