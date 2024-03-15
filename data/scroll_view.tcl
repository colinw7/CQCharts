set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set nx 4
set ny 4

set dx [expr {100.0 / $nx}]
set dy [expr {100.0 / $ny}]

set dx1 [expr {3*$dx}]
set dy1 [expr {3*$dy}]

set ip 0

for {set r 0} {$r < $ny} {incr r} {
  set vx1 [expr {$r*$dx1}]
  set vx2 [expr {$vx1 + $dx1}]

  for {set c 0} {$c < $nx} {incr c} {
    set vy1 [expr {$c*$dy1}]
    set vy2 [expr {$vy1 + $dy1}]

    set iy [expr {($ip % 3) + 1}]

    set plot [create_charts_plot -type xy -columns [list [list x 0] [list y $iy]] -title "XY Plot"]

    set_charts_property -plot $plot -name range.view -value [list $vx1 $vy1 $vx2 $vy2]

    incr ip
  }
}

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name scroll.plots -value 1
