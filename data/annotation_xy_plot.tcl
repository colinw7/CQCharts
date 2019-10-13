set plot [create_charts_plot -type empty -title "Cos/Sin Curve" -xmin 0 -xmax 360 -ymin -1 -ymax 1]

set pi [expr {atan(1.0)*4}]

set n 20

set cpoints {}

for {set i 0} {$i <= $n} {incr i} {
  set x [expr {(360.0*$i)/$n}]
  set r [expr {$pi*($x/180.0)}]

  set y [expr {cos($r)}]

  lappend cpoints [list $x $y]
}

set cpoints1 {}
lappend cpoints1 [list 0 0]
for {set i 0} {$i <= $n} {incr i} {
  lappend cpoints1 [lindex $cpoints $i]
}
lappend cpoints1 [list 360 0]

set spoints {}

for {set i 0} {$i <= $n} {incr i} {
  set x [expr {(360.0*$i)/$n}]
  set r [expr {$pi*($x/180.0)}]

  set y [expr {sin($r)}]

  lappend spoints [list $x $y]
}

set cpoly [create_charts_polygon_annotation -plot $plot -points $cpoints1 -tip "COS Curve"]
set spoly [create_charts_polygon_annotation -plot $plot -points $spoints -tip "Sin Curve"]

set_charts_property -annotation $cpoly -name stroke.visible -value 0
set_charts_property -annotation $cpoly -name fill.color -value "#ddffdd"
set_charts_property -annotation $cpoly -name fill.alpha -value 0.5

set_charts_property -annotation $spoly -name stroke.visible -value 0
set_charts_property -annotation $spoly -name fill.alpha -value 0.5
set_charts_property -annotation $spoly -name fill.color -value "#ddddff"

set cline [create_charts_polyline_annotation -plot $plot -points $cpoints]
set sline [create_charts_polyline_annotation -plot $plot -points $spoints]

for {set i 0} {$i <= $n} {incr i} {
  set cp [create_charts_point_annotation -plot $plot -position [lindex $cpoints $i]]
}

for {set i 0} {$i <= $n} {incr i} {
  set sp [create_charts_point_annotation -plot $plot -position [lindex $spoints $i]]
}

set_charts_property -plot $plot -name xaxis.position.value -value 0.0
