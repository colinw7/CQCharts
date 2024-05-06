proc annotationSlot { annotation id } {
  echo "annotationSlot: $annotation $id"
}

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set annotation [create_charts_symbol_map_key_annotation -plot $plot]

set_charts_property -annotation $annotation -name key.position -value {50 50 P}
set_charts_property -annotation $annotation -name key.mapMin   -value 16
set_charts_property -annotation $annotation -name key.mapMax   -value 64

connect_charts_signal -annotation $annotation -from annotationPressed -to annotationSlot
