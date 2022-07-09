set n 100

set PI [expr {4.0*atan(1)}]

set xmin -$PI
set xmax $PI
set ymin -$PI
set ymax $PI

set points {}

for {set ix 0} {$ix < $n} {incr ix} {
  set x [expr {($xmax - $xmin)*(1.0*$ix)/($n - 1) + $xmin}]

  for {set iy 0} {$iy < $n} {incr iy} {
    set y [expr {($ymax - $ymin)*(1.0*$iy)/($n - 1) + $ymin}]

    set z [expr {sin($x)*cos($y)}]

    lappend points [list $x $y $z]
  }
}

set plotId [create_charts_plot -type empty -xmin $xmin -ymin $ymin -xmax $xmax -ymax $ymax]

set ann [create_charts_point3d_set_annotation -plot $plotId -points $points]
