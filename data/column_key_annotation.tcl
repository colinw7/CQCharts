proc annotationSlot { annotation id } {
  echo "annotationSlot: $annotation $id"
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -type scatter -model $model \
 -columns {{x sepalLength} {y sepalWidth} {group species}}]

set_charts_property -plot $plot -name key.visible -value 0

set annotation [create_charts_key_annotation -plot $plot -column 4]

connect_charts_signal -annotation $annotation -from annotationIdPressed -to annotationSlot
