proc annotationSlot { annotation id } {
  echo "annotationSlot: $annotation $id"
}

set plot [create_charts_plot -type empty -title "Key Annotation" \
 -xmin -1.0 -xmax 1.0 -ymin -1.0 -ymax 1.0]

set annotation [create_charts_key_annotation -plot $plot]

# text and color
add_charts_key_item -annotation $annotation -text Red   -color red
add_charts_key_item -annotation $annotation -text Green -color green
add_charts_key_item -annotation $annotation -text Blue  -color blue

# text and symbol
add_charts_key_item -annotation $annotation -text Cross  -symbol cross  -color red
add_charts_key_item -annotation $annotation -text Circle -symbol circle -color green
add_charts_key_item -annotation $annotation -text Box    -symbol box    -color blue

# gradient
add_charts_key_item -annotation $annotation -text Gradient -gradient moreland -id "gradient"

connect_charts_signal -annotation $annotation -from annotationPressed -to annotationSlot

echo [get_charts_property -key_item "keyitem:gradient:gradient" -name minValue]
echo [get_charts_property -key_item "keyitem:gradient:gradient" -name maxValue]

set_charts_property -key_item "keyitem:gradient:gradient" -name minValue -value 5.0
set_charts_property -key_item "keyitem:gradient:gradient" -name maxValue -value 10.0
