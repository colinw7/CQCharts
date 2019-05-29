proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts $pos

  set rect [get_charts_property -annotation $id -name rect]
  puts $rect

  set text [get_charts_property -annotation $id -name text]
  puts $text
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "This plot has a very long title which we might want to format on multiple lines"]

set viewId [get_charts_property -plot $plotId -name viewId]

set textAnnotationId1 [create_charts_text_annotation -plot $plotId -id one -rect {0 -60 200 -50} -text "One"]

set textAnnotationId2 [create_charts_text_annotation -plot $plotId -id two -rect {0 -50 200 -40} -text "Two" -color red]

set textAnnotationId3 [create_charts_text_annotation -plot $plotId -id three -rect {0 -40 200 -30} -text "Three" -color green -alpha 0.5]

set textAnnotationId4 [create_charts_text_annotation -plot $plotId -id four -rect {0 -30 200 -20} -text "Four"  -color blue  -angle 45]

set textAnnotationId5 [create_charts_text_annotation -plot $plotId -id long -rect {-200 0 200 40} -text "The rain in spain falls mainly on the plain. She sell sea shells on the sea shore. The quick brown fox jumps over the lazy dog." -stroked 1 -filled 1 -corner_size 12]

set textAnnotationId6 [create_charts_text_annotation -plot $plotId -id long -rect {100 100 400 400 pixel} -text "<table><tr><td>One</td><td>Two</td></tr><tr><td>Three</td><td>Four</td></tr></table>" -stroked 1 -filled 1 -corner_size 12 -html]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
