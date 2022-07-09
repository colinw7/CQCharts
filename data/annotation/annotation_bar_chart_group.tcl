set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set maxv [get_charts_data -model $model -column 1 -name details.max]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set nr1 [expr {$nr - 1}]

set plot [create_charts_plot -type empty -title "Under 5 Years by State" \
  -xmin -0.5 -xmax [expr {$nr - 0.5}] -ymin 0 -ymax $maxv]

set group [create_charts_annotation_group -plot $plot -id group]

set_charts_property -annotation $group -name rectangle \
  -value [list [list 0 0] [list $nr1 $maxv]]

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

  set rvalue [expr {(1.0*$x)/($::nr - 1)}]
  set color  "palette $rvalue"

  set_charts_property -annotation $rect -name fill.color -value $color
  set_charts_property -annotation $rect -name margin -value "4px 0px 4px 0px"

  return $rect
}

set x 0

for {set r 0} {$r < $nr} {incr r} {
  set name  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set bar [create_rect $plot $group $name $x $value]

  set_charts_data -annotation $xaxis -name tick_label -value [list $x $name]

  incr x
}

set_charts_property -annotation $group -name layout.type  -value HV
set_charts_property -annotation $group -name layout.align -value {AlignHCenter|AlignBottom}

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0
