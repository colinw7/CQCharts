proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

connect_charts_signal -plot $plotId -from annotationPressed -to annotationSlot

create_charts_point_annotation -plot $plotId -id dot       -position {100 -60} -symbol dot       -size 16px
create_charts_point_annotation -plot $plotId -id cross     -position {100 -50} -symbol cross     -size 16px
create_charts_point_annotation -plot $plotId -id plus      -position {100 -40} -symbol plus      -size 16px
create_charts_point_annotation -plot $plotId -id y         -position {100 -30} -symbol y         -size 16px
create_charts_point_annotation -plot $plotId -id triangle  -position {100 -20} -symbol triangle  -size 16px
create_charts_point_annotation -plot $plotId -id itriangle -position {100 -10} -symbol itriangle -size 16px
create_charts_point_annotation -plot $plotId -id box       -position {100   0} -symbol box       -size 16px -filled 1 -fill_color red
create_charts_point_annotation -plot $plotId -id diamond   -position {100  10} -symbol diamond   -size 16px
create_charts_point_annotation -plot $plotId -id star      -position {100  20} -symbol star      -size 16px
create_charts_point_annotation -plot $plotId -id star6     -position {100  30} -symbol star6     -size 16px
create_charts_point_annotation -plot $plotId -id circle    -position {100  40} -symbol circle    -size 16px -filled 1 -fill_color blue
create_charts_point_annotation -plot $plotId -id pentagon  -position {100  50} -symbol pentagon  -size 16px
create_charts_point_annotation -plot $plotId -id ipentagon -position {100  60} -symbol ipentagon -size 16px
