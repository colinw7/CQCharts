proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

set viewId [get_charts_property -plot $plotId -name viewId]

set vrectId1 [create_charts_rectangle_annotation -view $viewId -id rectId1 -tip "Rectangle 1" \
  -start {10 10} -end {50 50} \
  -filled 1 -fill_color blue -fill_alpha 0.5]

set rectId1 [create_charts_rectangle_annotation -plot $plotId -id rectId1 -tip "Rectangle 1" \
  -start {0 -60} -end {100 -40} \
  -filled 1 -fill_color red -fill_alpha 0.5]

set rectId2 [create_charts_rectangle_annotation -plot $plotId -id rectId2 -tip "Rectangle 2" \
  -start {0 -30} -end {100 -10} \
  -stroke_color green -stroke_alpha 0.5 -stroke_width 3 -corner_size 8]

set rectId3 [create_charts_rectangle_annotation -plot $plotId -id rectId3 -tip "Rectangle 3" \
  -start {0 0} -end {100 20} \
  -stroke_color blue -stroke_alpha 0.5 -stroke_width 3 -border_sides tl]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot
