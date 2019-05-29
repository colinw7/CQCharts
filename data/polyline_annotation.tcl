proc annotationSlot { view plot id } {
  echo "annotationSlot: $view $plot $id"
}

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations" -xmin -100 -xmax 100 -ymin -100 -ymax 100]

connect_charts_signal -plot $plotId -from annotationIdPressed -to annotationSlot

set polyId [create_charts_polyline_annotation -plot $plotId -points "-30 -30 -20 -20 -10 10 0 -10 10 -20 20 30 30 10" \
 -filled 1 -fill_color red -fill_alpha 0.5]
