set plotId [create_charts_plot -type empty -xmin 140 -ymin 80 -xmax 220 -ymax 160]

set pathStr "M 173.114 146.37 L 173.09 146.43 L 192.31 98.79 L 196.111 89.1898"

if {1} {
  set arrowId [create_charts_arrow_annotation -plot $plotId -start {10 90} -end {90 10} \
    -fhead stealth -thead line -length 80px -line_width 20px \
    -stroke_color {palette 0.3} -fill_color {palette 0.3}]

  set_charts_property -annotation $arrowId -name path -value $pathStr
  set_charts_property -annotation $arrowId -name line.rectilinear -value 1
  set_charts_property -annotation $arrowId -name debugLabels -value 1
} else {
  set pathId [create_charts_path_annotation -plot $plotId -id path -path $pathStr]
}
