proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot

create_charts_point_annotation -plot $plotId -id dot       -position {100 -60} -type dot       -size 16px
create_charts_point_annotation -plot $plotId -id cross     -position {100 -50} -type cross     -size 16px
create_charts_point_annotation -plot $plotId -id plus      -position {100 -40} -type plus      -size 16px
create_charts_point_annotation -plot $plotId -id y         -position {100 -30} -type y         -size 16px
create_charts_point_annotation -plot $plotId -id triangle  -position {100 -20} -type triangle  -size 16px
create_charts_point_annotation -plot $plotId -id itriangle -position {100 -10} -type itriangle -size 16px
create_charts_point_annotation -plot $plotId -id box       -position {100   0} -type box       -size 16px -filled 1 -fill_color red
create_charts_point_annotation -plot $plotId -id diamond   -position {100  10} -type diamond   -size 16px
create_charts_point_annotation -plot $plotId -id star      -position {100  20} -type star      -size 16px
create_charts_point_annotation -plot $plotId -id star6     -position {100  30} -type star6     -size 16px
create_charts_point_annotation -plot $plotId -id circle    -position {100  40} -type circle    -size 16px -filled 1 -fill_color blue
create_charts_point_annotation -plot $plotId -id pentagon  -position {100  50} -type pentagon  -size 16px
create_charts_point_annotation -plot $plotId -id ipentagon -position {100  60} -type ipentagon -size 16px
