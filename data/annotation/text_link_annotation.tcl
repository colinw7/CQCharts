proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set text [get_charts_property -annotation $id -name text]
  puts "text: $text"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set viewId [get_charts_property -plot $plotId -name state.viewId]

set textAnnotationId [create_charts_text_annotation -plot $plotId -id one \
  -position {50 50} -text "Text Link"]

set_charts_property -annotation $textAnnotationId -name isLink -value 1

connect_charts_signal -plot $plotId -from annotationPressed -to annotationSlot
