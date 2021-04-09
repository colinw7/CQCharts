proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set image [get_charts_property -annotation $id -name image]
  puts "image: $image"
}

set modelId [load_charts_model -csv data/p2.csv]

set plotId [create_charts_plot -type scatter -columns {{x 0} {y 1}}]

set viewId [get_charts_property -plot $plotId -name viewId]

set image1 [create_charts_image_annotation -plot $plotId -id one \
  -position {0.25 0.25} -svg "data/beer_bottle.svg"]
set image2 [create_charts_image_annotation -plot $plotId -id two \
  -position {0.75 0.25} -image "data/Catwoman.jpg"]
set image3 [create_charts_image_annotation -plot $plotId -id three \
  -position {0.75 0.75} -icon "data/beer_bottle.svg"]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
