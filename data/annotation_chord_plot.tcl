proc tcl::mathfunc::max { a b } {
  if {$a > $b} {
    return $a
  } else {
    return $b
  }
}

proc tcl::mathfunc::deg_to_rad { d } {
  return [expr {3.141592653*$d/180.0}]
}

proc tcl::mathfunc::dcos { d } {
  return [expr {cos(deg_to_rad($d))}]
}

proc tcl::mathfunc::dsin { d } {
  return [expr {sin(deg_to_rad($d))}]
}

set model [load_charts_model -csv data/sankey.csv -comment_header -column_type {{{0 name_pair}}}]

set maxv [get_charts_data -model $model -column 1 -name details.max]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set nameInd {}
set ind     1
set edges   {}

for {set r 0} {$r < $nr} {incr r} {
  set path  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set names [split $path "/"]

  set from [lindex $names 0]
  set to   [lindex $names 1]

  if {! [dict exists $nameInd $from]} {
    dict set nameInd $from $ind 
    incr ind
  }

  if {! [dict exists $nameInd $to]} {
    dict set nameInd $to $ind 
    incr ind
  }

  lappend edges [list $from $to $value $r]
}

dict for { name ind } $nameInd {
  set fromTotal($ind) 0.0
  set toTotal($ind) 0.0
  set maxTotal($ind) 0.0

  set indAngle($ind) 0.0
  set indSpan($ind) 0.0
}

foreach edge $edges {
  set from  [lindex $edge 0]
  set to    [lindex $edge 1]
  set value [lindex $edge 2]

  set fromInd [dict get $nameInd $from]
  set toInd   [dict get $nameInd $to]

  set fromTotal($fromInd) [expr {$fromTotal($toInd) + $value}]
  set toTotal($toInd) [expr {$toTotal($toInd) + $value}]
}

set sumTotal 0.0

dict for { name ind } $nameInd {
  set maxTotal($ind) [expr {max($fromTotal($ind), $toTotal($ind))}]

  #echo "$ind : $fromTotal($ind) $toTotal($ind) $maxTotal($ind)"

  set sumTotal [expr {$sumTotal + $maxTotal($ind)}]
}

set plot [create_charts_plot -model $model -type empty -xmin -1 -xmax 1 -ymin -1 -ymax 1]

set nn [llength [dict keys $nameInd]]

set gapAngle    1.0
set angleExtent [expr {360.0 - $nn*$gapAngle}]

set a1 90.0
set da [expr {1.0*$angleExtent/$sumTotal}]

set innerRadius 0.9
set outerRadius 1.0

set midRadius [expr {($innerRadius + $outerRadius)/2.0}]

dict for { name ind } $nameInd {
  set da1 [expr {$da*$maxTotal($ind)}]

  set id "arc_${name}"
  #set tip "$name $ind ($a1 $da1)"
  set tip "$name"

  set arc [create_charts_pie_slice_annotation -plot $plot -id $id -tip $tip -position {0 0} \
   -inner_radius $innerRadius -outer_radius $outerRadius -start_angle $a1 -span_angle $da1]

  set color  [expr {1.0*$maxTotal($ind)/$sumTotal}]
  set ncolor "palette $color"

  set_charts_property -annotation $arc -name fill.color -value $ncolor

  set indAngle($ind) $a1
  set indSpan($ind) $da1

  set am [expr {$a1 + $da1/2.0}]

  set tx [expr {$midRadius*dcos($am)}]
  set ty [expr {$midRadius*dsin($am)}]

  set id "text_arc_${name}"

  set arc_text [create_charts_text_annotation -plot $plot -id $id \
                  -position [list $tx $ty] -text $name]

  set a1 [expr {$a1 + $da1} + $gapAngle]
}

dict for { name ind } $nameInd {
  set currentIAngle($ind) 0.0
  set currentOAngle($ind) 0.0
}

foreach edge $edges {
  set from  [lindex $edge 0]
  set to    [lindex $edge 1]
  set value [lindex $edge 2]

  set fromInd [dict get $nameInd $from]
  set toInd   [dict get $nameInd $to]

  set fromAngle [expr {$indAngle($fromInd) + $currentIAngle($fromInd)}]
  set toAngle [expr {$indAngle($toInd) + $currentOAngle($toInd)}]

  set segmentAngle [expr {$da*$value}]
  #echo "$fromInd $segmentAngle"

  set id  "${from}:${to}"
  #set tip "$from ($fromInd) -> $to ($toInd) = $value ($fromAngle $segmentAngle $toAngle)"
  set tip "$from -> $to = $value"

  set connector [create_charts_arc_connector_annotation -plot $plot -id $id -tip $tip \
                   -center [list 0 0] -radius $innerRadius \
                   -src_start_angle $fromAngle -src_span_angle $segmentAngle \
                   -dest_start_angle $toAngle -dest_span_angle $segmentAngle]

  set currentIAngle($fromInd) [expr {$currentIAngle($fromInd) + $segmentAngle}]
  set currentOAngle($toInd) [expr {$currentOAngle($toInd) + $segmentAngle}]

  set color [expr {1.0*$value/$maxv}]

  set ncolor "palette $color"

  set_charts_property -annotation $connector -name fill.color -value $ncolor
  set_charts_property -annotation $connector -name fill.alpha -value 0.3
}
