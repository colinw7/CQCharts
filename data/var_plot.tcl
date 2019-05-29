set n 100

if {0} {
for {set i 0} {$i < $n} {incr i} {
  set x($i) $i
  set y($i) [expr {$i/20.0}]
  set c($i) [expr {cos($y($i))}]
  set s($i) [expr {sin($y($i))}]
}
}

for {set i 0} {$i < $n} {incr i} {
  set i1 [expr {$i/20.0}]

  lappend x $i
  lappend y $i1
  lappend c [expr {cos($i1)}]
  lappend s [expr {sin($i1)}]
}

set modelInd [load_charts_model -var x -var y -var c -var s]

set plotInd [create_charts_plot -type xy -columns {{x 0} {y {2 3}}}]
