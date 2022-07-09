proc annotationSlot { view plot id } {
  echo "annotationSlot: $view $plot $id"
}

set plot [create_charts_plot -type empty -title "Polyline Annotation" \
  -xmin -100 -xmax 100 -ymin -100 -ymax 100]

connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot

set poly [create_charts_polyline_annotation -plot $plot \
 -points "-30 -30 -20 -20 -10 10 0 -10 10 -20 20 30 30 10"]

set_charts_property -plot $plot -name scaling.equal -value 1

set_charts_property -annotation $poly -name stroke.width -value "5P"
