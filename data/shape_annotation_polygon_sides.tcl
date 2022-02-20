set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

for {set i 3} {$i < 12} {incr i} {
  set x1 [expr {($i - 3)*10 + 10}]
  set y1 [expr {($i - 3)*10 + 10}]
  set x2 [expr {$x1 + 10}]
  set y2 [expr {$y1 + 10}]

  set r [list [list $x1 $y1] [list $x2 $y2]]

  set shape [create_charts_shape_annotation -plot $plot -rectangle $r]

  set_charts_property -annotation $shape -name shapeType -value POLYGON

  set_charts_property -annotation $shape -name numSides -value $i

  #---

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $i]

  set_charts_property -annotation $shape -name textInd -value $text

  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
  set_charts_property -annotation $text -name text.scaled -value 1
}
