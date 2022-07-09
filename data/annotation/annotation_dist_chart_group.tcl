set model [load_charts_model -csv data/gaussian.txt -comment_header]

set nr [get_charts_data -model $model -name num_rows]

set values {}

for {set r 0} {$r < $nr} {incr r} {
  set value [get_charts_data -model $model -column 0 -row $r -name value -role display]

  lappend values $value
}

set bvals [bucket_charts_values -values $values]

set nb [llength $bvals]

set nb1 [expr {$nb - 1}]

set minv [lindex [lindex $bvals 0] 0]
set maxv [lindex [lindex $bvals end] 1]

set maxn 0

foreach bv $bvals {
  set n [lindex $bv 2]

  if {$n > $maxn} {
    set maxn $n
  }
}

set plot [create_charts_plot -type empty -title "Distribution" \
  -xmin -0.5 -xmax [expr {$nb - 0.5}] -ymin 0 -ymax $maxn]

set group [create_charts_annotation_group -plot $plot -id group]

set_charts_property -annotation $group -name rectangle \
  -value [list [list 0 0] [list $nb1 $maxn]]

set xaxis [create_charts_axis_annotation -plot $plot \
 -direction horizontal -start 0.0 -end 0.001 -position 0.0]
set yaxis [create_charts_axis_annotation -plot $plot \
 -direction vertical -start 0.0 -end 0.001 -position 0.0]

set_charts_property -annotation $xaxis -name objRef    -value $group
set_charts_property -annotation $xaxis -name valueType -value INTEGER
set_charts_property -annotation $yaxis -name objRef    -value $group
set_charts_property -annotation $yaxis -name valueType -value INTEGER

proc create_rect { plot group name x value } {
  set x1 [expr {$x - 0.5}]
  set x2 [expr {$x + 0.5}]

  if {$name != ""} {
    set id  $name
    set tip "$name : $value"
  } else {
    set id  "rect$x"
    set tip $value
  }

  set rect [create_charts_rectangle_annotation -plot $plot -group $group -id $id -tip $tip \
    -rectangle [list $x1 0 $x2 $value]]

  if {$name != ""} {
    set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

    set_charts_property -annotation $rect -name textInd -value $text

    set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
    set_charts_property -annotation $text -name text.scaled -value 1
  }

  set rvalue [expr {(1.0*$x)/($::nb - 1)}]
  set color  "palette $rvalue"

  set_charts_property -annotation $rect -name fill.color -value $color
  set_charts_property -annotation $rect -name margin -value "4px 0px 4px 0px"

  return $rect
}

set x 0

foreach bv $bvals {
  set v1 [lindex $bv 0]
  set v2 [lindex $bv 1]
  set n  [lindex $bv 2]

  set name [format "\[%g,%g)" $v1 $v2]

  set bar [create_rect $plot $group "" $x $n]

  set_charts_data -annotation $xaxis -name tick_label -value [list $x $name]

  incr x
}

set_charts_property -annotation $group -name layout.type  -value HV
set_charts_property -annotation $group -name layout.align -value {AlignHCenter|AlignBottom}

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0
