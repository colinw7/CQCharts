proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  #set pos [get_charts_property -annotation $id -name position]
  #puts "pos: $pos"

  #set image [get_charts_property -annotation $id -name image]
  #puts "image: $image"
}

set modelId [load_charts_model -csv data/p2.csv]

set plotId [create_charts_plot -type scatter -columns {{x 0} {y 1}}]

set viewId [get_charts_property -plot $plotId -name viewId]

set group [create_charts_annotation_group -plot $plotId -id group]

set image [create_charts_image_annotation -plot $plotId -id image \
  -rectangle {0 0 0.5 0.5} -svg "data/beer_bottle.svg" -group $group]
set text [create_charts_text_annotation -plot $plotId -id test \
  -position {0 0} -text "Beer Bottle" -group $group]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
