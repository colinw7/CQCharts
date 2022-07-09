proc mapx { x xmin xmax omin omax } {
  return [expr {1.0*($x - $xmin)*($omax - $omin)/($xmax - $xmin) + $omin}]
}

set model [load_charts_model -csv data/parallel_coords.csv -first_line_header]

set nr [get_charts_data -model $model -name num_rows   ]
set nr1 [expr {$nr - 1}]

set nc [get_charts_data -model $model -name num_columns]
set nc1 [expr {$nc - 1}]

set plot [create_charts_plot -model $model -type empty -xmin 0 -xmax $nc1 -ymin 0 -ymax 1]

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

for {set c 0} {$c < $nc} {incr c} {
  set type [get_charts_data -model $model -column $c -name details.type]

  set minv [get_charts_data -model $model -column $c -name details.min]
  set maxv [get_charts_data -model $model -column $c -name details.max]

  set id  $c
  set tip $c

  set axis [create_charts_axis_annotation -plot $plot -id $id -tip $tip \
             -direction vertical -start 0.0 -end 1.0 -position $c]

  if {$type != "string"} {
    set_charts_property -annotation $axis -name valueStart -value $minv
    set_charts_property -annotation $axis -name valueEnd   -value $maxv
  } else {
    set_charts_property -annotation $axis -name valueType      -value INTEGER
    set_charts_property -annotation $axis -name majorIncrement -value 1

    set nu     [get_charts_data -model $model -column $c -name details.num_unique]
    set unique [get_charts_data -model $model -column $c -name details.unique_values]

    set nu1 [expr {$nu - 1}]

    set_charts_property -annotation $axis -name valueStart -value 0
    set_charts_property -annotation $axis -name valueEnd   -value $nu1

    set iu 0

    foreach value $unique {
      set_charts_data -annotation $axis -name tick_label -value [list $iu $value]

      incr iu
    }
  }
}

for {set r 0} {$r < $nr} {incr r} {
  set points {}

  for {set c 0} {$c < $nc} {incr c} {
    set type [get_charts_data -model $model -column $c -name details.type]

    set value [get_charts_data -model $model -column $c -row $r -name value -role display]

    if {$type == "string"} {
      set nu [get_charts_data -model $model -column $c -name details.num_unique]
      set id [get_charts_data -model $model -column $c -name details.unique_id -data $value]

      set id1 [mapx $id 0 $nu 0.0 1.0]

      lappend points [list $c $id1]
    } else {
      if {$value == ""} {
        continue
      }

      set minv [get_charts_data -model $model -column $c -name details.min]
      set maxv [get_charts_data -model $model -column $c -name details.max]

      set x1 [mapx $value $minv $maxv 0.0 1.0]

      lappend points [list $c $x1]
    }
  }

  set color  [expr {1.0*$r/($nr - 1)}]
  set ncolor "palette $color"

  set id $r
  set tip $r

  set poly [create_charts_polyline_annotation -plot $plot -points $points -id $id -tip $tip]

  set_charts_property -annotation $poly -name stroke.color -value $ncolor
}

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0
