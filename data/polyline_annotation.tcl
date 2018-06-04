set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "Annotations" -xmin -100 -xmax 100 -ymin -100 -ymax 100]

set polyId [create_polyline_shape -plot $plotId -points "-30 -30 -20 -20 -10 10 0 -10 10 -20 20 30 30 10" \
 -background 1 -background_color red -background_alpha 0.5]
