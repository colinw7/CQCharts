proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_property -view $viewId -plot $plotId -annotation $id -name position]
  puts $pos

  set text [get_property -view $viewId -plot $plotId -annotation $id -name text]
  puts $text
}

set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "This plot has a very long title which we might want to format on multiple lines"]

set viewId [get_property -plot $plot1Id -name viewId]

set textAnnotationId1 [text_shape -plot $plotId -id one -x 0 -y -60 -text "One"]

set textAnnotationId2 [text_shape -plot $plotId -id two -x 0 -y -50 -text "Two" -color red]

set textAnnotationId3 [text_shape -plot $plotId -id three -x 0 -y -40 -text "Three" -color green -alpha 0.5]

set textAnnotationId4 [text_shape -plot $plotId -id four -x 0 -y -30 -text "Four"  -color blue  -angle 45]

set textAnnotationId5 [text_shape -plot $plotId -id long -x 0 -y 0 -text "The rain in spain falls mainly on the plain. She sell sea shells on the sea shore. The quick brown fox jumps over the lazy dog." -border 1 -background 1 -corner_size 12]

connect -view $viewId -plot $plotId -from annotationIdPressed -to annotationSlot
