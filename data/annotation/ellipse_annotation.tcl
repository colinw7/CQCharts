proc annotationSlot { view plot id } {
  echo "annotationSlot: $view $plot $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations" -xmin -100 -xmax 100 -ymin -100 -ymax 100]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot

set ellipseId1 [create_charts_ellipse_annotation -plot $plotId -id ellipseId1 -center {0 -60} -rx 50 -ry 50 -stroked 1 -stroke_color red -stroke_alpha 0.5]

set ellipseId2 [create_charts_ellipse_annotation -plot $plotId -id ellipseId2 -center {0 -30} -rx 50 -ry 30 -stroke_color green -stroke_alpha 0.5 -stroke_width 3px]

set ellipseId3 [create_charts_ellipse_annotation -plot $plotId -id ellipseId3 -center {0   0} -rx 30 -ry 50 -stroke_color blue -stroke_alpha 0.5 -stroke_width 3px]
