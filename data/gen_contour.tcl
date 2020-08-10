set n 100

set PI [expr {4.0*atan(1)}]

set xmin -$PI
set xmax $PI
set ymin -$PI
set ymax $PI

set x {}
set y {}
set z {}

for {set ix 0} {$ix < $n} {incr ix} {
  set x1 [expr {($xmax - $xmin)*(1.0*$ix)/($n - 1) + $xmin}]

  lappend x $x1
}

for {set iy 0} {$iy < $n} {incr iy} {
  set y1 [expr {($ymax - $ymin)*(1.0*$iy)/($n - 1) + $ymin}]

  lappend y $y1
}

set rows {}

set row {}

lappend row 0.0

for {set iy 0} {$iy < $n} {incr iy} {
  set y1 [expr {($ymax - $ymin)*(1.0*$iy)/($n - 1) + $ymin}]

  lappend row $y1
}

lappend rows $row

for {set ix 0} {$ix < $n} {incr ix} {
  set x1 [expr {($xmax - $xmin)*(1.0*$ix)/($n - 1) + $xmin}]

  set row {}

  lappend row $x1

  for {set iy 0} {$iy < $n} {incr iy} {
    set y1 [expr {($ymax - $ymin)*(1.0*$iy)/($n - 1) + $ymin}]

    set z1 [expr {sin($x1)*cos($y1)}]

    lappend z $z1

    lappend row $z1
  }

  lappend rows $row
}

foreach row $rows {
  set i 0

  foreach v $row {
    if {$i > 0} {
      puts -nonewline ","
    }

    puts -nonewline $v

    incr i
  }

  puts ""
}
