set plotId [create_charts_plot -type empty \
  -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set rectId [create_charts_rectangle_annotation -plot $plotId -id rectId1 -tip "Rectangle" \
  -start {25 25} -end {75 75} \
  -filled 1 -fill_color red -fill_alpha 0.5]

set_charts_property -annotation $rectId -name fill.pattern -value "palette:plasma"
