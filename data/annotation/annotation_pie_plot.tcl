set model [load_charts_model -csv data/ages.csv -first_line_header -column_type {{{1 integer}}}]

set plot [create_charts_plot -model $model -type empty -xmin -1 -xmax 1 -ymin -1 -ymax 1]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set sum [get_charts_data -model $model -column 1 -name details.sum]

proc create_arc { plot a da name value } {
  set id  $name
  set tip "$name : $value"

  set arc [create_charts_pie_slice_annotation -plot $plot -position {0 0} -id $id -tip $tip \
   -inner_radius 0.0 -outer_radius 1.0 -start_angle $a -span_angle $da]

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $arc  -name textInd -value $text
  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
# set_charts_property -annotation $text -name text.scaled -value 1
  set_charts_property -annotation $text -name text.font -value -8

  set rvalue [expr {1.0*$value/$::sum}]

  set color "palette $rvalue"

  set_charts_property -annotation $arc -name fill.color -value $color

  return $arc
}

set a 0.0

for {set r 0} {$r < $nr} {incr r} {
  set name  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set rvalue [expr {1.0*$value/$sum}]

  set da [expr {$rvalue*360.0}]

  set arc [create_arc $plot $a $da $name $value]

  set a [expr {$a + $da}]
}

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
