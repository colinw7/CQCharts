proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"
}

set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "Annotations"]

connect_chart -plot $plotId -from annotationIdPressed -to annotationSlot

create_point_shape -plot $plotId -id dot       -x 100 -y -60 -type dot       -size 16
create_point_shape -plot $plotId -id cross     -x 100 -y -50 -type cross     -size 16
create_point_shape -plot $plotId -id plus      -x 100 -y -40 -type plus      -size 16
create_point_shape -plot $plotId -id y         -x 100 -y -30 -type y         -size 16
create_point_shape -plot $plotId -id triangle  -x 100 -y -20 -type triangle  -size 16
create_point_shape -plot $plotId -id itriangle -x 100 -y -10 -type itriangle -size 16
create_point_shape -plot $plotId -id box       -x 100 -y   0 -type box       -size 16 -filled 1 -fill_color red
create_point_shape -plot $plotId -id diamond   -x 100 -y  10 -type diamond   -size 16
create_point_shape -plot $plotId -id star      -x 100 -y  20 -type star      -size 16
create_point_shape -plot $plotId -id star6     -x 100 -y  30 -type star6     -size 16
create_point_shape -plot $plotId -id circle    -x 100 -y  40 -type circle    -size 16 -filled 1 -fill_color blue
create_point_shape -plot $plotId -id pentagon  -x 100 -y  50 -type pentagon  -size 16
create_point_shape -plot $plotId -id ipentagon -x 100 -y  60 -type ipentagon -size 16
