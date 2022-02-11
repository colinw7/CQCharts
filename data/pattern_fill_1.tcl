# Use -Dtype=<type>

if {! [info exists type]} {
  set type DOTS
}

echo $type

proc rand_in { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

set plotId [create_charts_plot -type empty \
  -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set nb 10

set x1 1
set y1 1
set y2 5
set dx [expr {100/$nb}]

set args "altColor=#eeeeee"

for {set ib 0} {$ib < $nb} {incr ib} {
  set x2 [expr {$x1 + $dx}]
  set y2 [rand_in 5 100]

  set rectId [create_charts_rectangle_annotation -plot $plotId -id rectId1 -tip "Rectangle" \
    -start [list $x1 $y1] -end [list $x2 $y2]]

  #set_charts_property -annotation $rectId -name stroke.color -value "#518cb1"
  set_charts_property -annotation $rectId -name stroke.color -value "#282828"
  set_charts_property -annotation $rectId -name fill.color -value "#afd4ce"

  set a [rand_in 0 359]
  set r [rand_in 4 8]
  set d [rand_in 4 8]

  #set_charts_property -annotation $rectId -name fill.pattern -value "bdiag,angle=45"
  set_charts_property -annotation $rectId -name fill.pattern -value "$type,angle=$a,radius=$r,delta=$d,$args"

  set x1 $x2
}
