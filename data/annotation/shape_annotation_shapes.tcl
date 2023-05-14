set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set shapes [get_charts_data -name shapes]

set nshapes [llength $shapes]

set nr [expr {int(sqrt($nshapes))}]
set nc [expr {int(($nshapes + $nr - 1)/$nr)}]

set dx [expr {100.0/$nc}]
set dy [expr {100.0/$nr}]

set dx1 [expr {$dx/10.0}]
set dy1 [expr {$dy/10.0}]

set i 0
set y 0

for {set r 0} {$r < $nr} {incr r} {
  set x 0

  for {set c 0} {$c < $nc} {incr c} {
    set shape [lindex $shapes $i]

    set x1 [expr {$x + $dx1}]
    set y1 [expr {$y + $dy1}]
    set x2 [expr {$x + $dx - 2*$dx1}]
    set y2 [expr {$y + $dy - 2*$dy1}]

    set rect [list [list $x1 $y1] [list $x2 $y2]]

    set ashape [create_charts_shape_annotation -plot $plotId -rectangle $rect]

    set_charts_property -annotation $ashape -name shapeType -value $shape
    set_charts_property -annotation $ashape -name fill.color -value #628ab5

    set x [expr {$x + $dx}]

    incr i
  }

  set y [expr {$y + $dy}]
}
