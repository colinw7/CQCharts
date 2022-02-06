proc rand_in { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

set plotId [create_charts_plot -type empty \
  -xmin 0 -xmax 100 -ymin 0 -ymax 100]

#set types { SOLID HATCH DENSE HORIZ VERT FDIAG BDIAG LGRADIENT RGRADIENT PALETTE IMAGE TEXTURE MASK }
set types [get_charts_data -name pattern_names]

set nt [llength $types]

set x1 1
set y1 1
set y2 5
set dx [expr {100/$nt}]

set args "altColor=#eeeeee,palette=plasma,image=image:/home/colinw/dev/progs/charts/data/CQCharts/pics/bldg.png"

foreach type $types {
  set x2 [expr {$x1 + $dx}]
  set y2 [rand_in 5 100]

  set rectId [create_charts_rectangle_annotation -plot $plotId -id rectId1 -tip "Rectangle" \
    -start [list $x1 $y1] -end [list $x2 $y2]]

  set_charts_property -annotation $rectId -name stroke.color -value "#518cb1"
  set_charts_property -annotation $rectId -name fill.color -value "#afd4ce"

  #set_charts_property -annotation $rectId -name fill.pattern -value "bdiag,angle=45"
  set_charts_property -annotation $rectId -name fill.pattern -value "$type,$args"

  set x1 $x2
}
