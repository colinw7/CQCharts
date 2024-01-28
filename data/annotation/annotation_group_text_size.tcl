proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  #set pos [get_charts_property -annotation $id -name position]
  #puts "pos: $pos"

  #set image [get_charts_property -annotation $id -name image]
  #puts "image: $image"
}

set modelId [load_charts_model -csv data/p2.csv]

set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 1 -ymax 1]

set viewId [get_charts_property -plot $plotId -name state.viewId]

set group [create_charts_annotation_group -plot $plotId -id group]

set text1 [create_charts_text_annotation -plot $plotId -id text1 \
  -rectangle {0.25 0.45 0.45 0.55} -text "Some Very Long Text" -group $group]
set text2 [create_charts_text_annotation -plot $plotId -id text2 \
  -rectangle {0.55 0.45 0.75 0.55} -text "Short Text" -group $group]

set_charts_property -annotation $group -name font.commonSize -value 1

set_charts_property -annotation $text1 -name fill.visible -value 0
set_charts_property -annotation $text1 -name stroke.visible -value 1
set_charts_property -annotation $text1 -name text.scaled -value 1
set_charts_property -annotation $text1 -name textAlign -value aligncenter

set_charts_property -annotation $text2 -name fill.visible -value 0
set_charts_property -annotation $text2 -name stroke.visible -value 1
set_charts_property -annotation $text2 -name text.scaled -value 1
set_charts_property -annotation $text2 -name textAlign -value aligncenter

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
