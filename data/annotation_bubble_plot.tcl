set model [load_charts_model -csv data/flare1.csv -comment_header -column_type {{{2 real}}}]

set maxv [get_charts_data -model $model -column 2 -name details.max]

set plot [create_charts_plot -model $model -type empty -xmin -10 -xmax 10 -ymin -10 -ymax 10]

set group [create_charts_annotation_group -plot $plot -id group]

proc create_ellipse { plot group name value } {
  #echo "create_ellipse $plot $group $name"

  set id  $name
  set tip "$name : $value"

  set ellipse [create_charts_ellipse_annotation -plot $plot -group $group -id $id -tip $tip \
    -center {0 0} -rx $value -ry $value]

  set_charts_property -annotation $ellipse -name value -value $value

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $ellipse -name textInd -value $text

  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
  set_charts_property -annotation $text -name text.scaled -value 1

  set_charts_property -annotation $ellipse -name state.editable -value 0
  set_charts_property -annotation $text    -name state.editable -value 0

  set color "palette $value"

  set_charts_property -annotation $ellipse -name fill.color -value $color

  return $ellipse
}

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

for {set r 0} {$r < $nr} {incr r} {
  set name  [get_charts_data -model $model -column 1 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 2 -row $r -name value -role display]

  set svalue [expr {$value/$maxv}]

  set ellipse [create_ellipse $plot $group $name $svalue]
}

set_charts_property -annotation $group -name layout.type -value CIRCLE

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
