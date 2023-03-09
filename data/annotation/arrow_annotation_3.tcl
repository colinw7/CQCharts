set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set pathStr "M 25 25 L 50 25 L 50 50 L 75 50"

if {1} {
  set arrowId [create_charts_arrow_annotation -plot $plotId -start {10 90} -end {90 10} \
    -fhead stealth -thead line -length 80px -line_width 20px \
    -stroke_color {palette 0.3} -fill_color {palette 0.3}]

  set_charts_property -annotation $arrowId -name path -value $pathStr
  set_charts_property -annotation $arrowId -name line.rectilinear -value 1
 #set_charts_property -annotation $arrowId -name debugLabels -value 1
} else {
  set pathId [create_charts_path_annotation -plot $plotId -id path -path $pathStr]
}
