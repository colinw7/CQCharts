proc tcl::mathfunc::deg_to_rad { d } {
  return [expr {3.141592653*$d/180.0}]
}

proc tcl::mathfunc::dcos { d } {
  return [expr {cos(deg_to_rad($d))}]
}

proc tcl::mathfunc::dsin { d } {
  return [expr {sin(deg_to_rad($d))}]
}

proc mapx { x xmin xmax omin omax } {
  return [expr {1.0*($x - $xmin)*($omax - $omin)/($xmax - $xmin) + $omin}]
}

set model [load_charts_model -csv data/radar.csv -first_line_header]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set ncol  0
set vcols {1 2 3 4 5}

set nvcols  [llength $vcols]
set nvcols1 [expr {$nvcols - 1}]

set da1 [expr {360.0/$nvcols}]

foreach vc $vcols {
  set max($vc) [get_charts_data -model $model -column $vc -name details.max]
}

set plot [create_charts_plot -model $model -type empty -xmin -1 -xmax 1 -ymin -1 -ymax 1]

for {set r 0} {$r < $nr} {incr r} {
  set name [get_charts_data -model $model -column $ncol -row $r -name value -role display]

  set a1 90

  set poly {}

  foreach vc $vcols {
    set value [get_charts_data -model $model -column $vc -row $r -name value -role display]

    set sv [mapx $value 0.0 $max($vc) 0.0 1.0]

    set x [expr {$sv*dcos($a1)}]
    set y [expr {$sv*dsin($a1)}]

    set a1 [expr {$a1 + $da1}]

    lappend poly [list $x $y]
  }

  set id $r
  set tip $r

  set poly [create_charts_polygon_annotation -plot $plot -id $id -tip $tip -points $poly]

  set color  [expr {1.0*$r/$nvcols1}]
  set ncolor "palette $color"

  set_charts_property -annotation $poly -name fill.color -value $ncolor
  set_charts_property -annotation $poly -name fill.alpha -value 0.5
}

set a1 90

foreach vc $vcols {
  set value [get_charts_data -model $model -column $vc -header -name value -role display]

  set x [expr {dcos($a1)}]
  set y [expr {dsin($a1)}]

  set a1 [expr {$a1 + $da1}]

  set text [create_charts_text_annotation -plot $plot -id $id -tip $tip \
              -position [list $x $y] -text $value]
}

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
