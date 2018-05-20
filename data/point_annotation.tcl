set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "Annotations"]

point_shape -plot $plotId -x 100 -y -60 -type dot       -size 16
point_shape -plot $plotId -x 100 -y -50 -type cross     -size 16
point_shape -plot $plotId -x 100 -y -40 -type plus      -size 16
point_shape -plot $plotId -x 100 -y -30 -type y         -size 16
point_shape -plot $plotId -x 100 -y -20 -type triangle  -size 16
point_shape -plot $plotId -x 100 -y -10 -type itriangle -size 16
point_shape -plot $plotId -x 100 -y   0 -type box       -size 16 -filled 1 -fill_color red
point_shape -plot $plotId -x 100 -y  10 -type diamond   -size 16
point_shape -plot $plotId -x 100 -y  20 -type star      -size 16
point_shape -plot $plotId -x 100 -y  30 -type star6     -size 16
point_shape -plot $plotId -x 100 -y  40 -type circle    -size 16 -filled 1 -fill_color blue
point_shape -plot $plotId -x 100 -y  50 -type pentagon  -size 16
point_shape -plot $plotId -x 100 -y  60 -type ipentagon -size 16
