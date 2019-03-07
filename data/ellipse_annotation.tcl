proc annotationSlot { view plot id } {
  echo "annotationSlot: $view $plot $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns "x=0,y=1" -title "Annotations" -xmin -100 -xmax 100 -ymin -100 -ymax 100]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot

set ellipseId1 [create_charts_ellipse_annotation -plot $plotId -id ellipseId1 -center {0 -60} -rx 50 -ry 50 -background 1 -background_color red -background_alpha 0.5]

set ellipseId2 [create_charts_ellipse_annotation -plot $plotId -id ellipseId2 -center {0 -30} -rx 50 -ry 30 -border_color green -border_alpha 0.5 -border_width 3px]

set ellipseId3 [create_charts_ellipse_annotation -plot $plotId -id ellipseId3 -center {0   0} -rx 30 -ry 50 -border_color blue -border_alpha 0.5 -border_width 3px]
