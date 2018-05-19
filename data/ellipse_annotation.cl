load_model -csv data/arrowstyle.csv
modelId = _rc

create_plot -type xy -columns "x=0,y=1" -title "Annotations" -xmin -100 -xmax 100 -ymin -100 -ymax 100
plotId = _rc

ellipse_shape -plot $plotId -xc 0 -yc -60 -rx 50 -ry 50 \
  -background 1 -background_color red -background_alpha 0.5
ellipseId1 = _rc

ellipse_shape -plot $plotId -xc 0 -yc -30 -rx 50 -ry 30 \
  -border_color green -border_alpha 0.5 -border_width 3px
ellipseId2 = _rc

ellipse_shape -plot $plotId -xc 0 -yc   0 -rx 30 -ry 50 \
  -border_color blue -border_alpha 0.5 -border_width 3px
ellipseId3 = _rc
