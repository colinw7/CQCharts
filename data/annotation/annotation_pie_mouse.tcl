set model [load_charts_model -csv data/ages.csv -first_line_header \
  -column_type {{{1 integer}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {values 1}} -title "pie chart"]

set_charts_property -plot $plot -name donut.visible -value 1

connect_charts_signal -plot $plot -from plotObjsAdded -to updateAnnotations

proc create_arc_group { plot obj } {
  set group_id ${obj}_arc_group_annotation

  set arc_group [create_charts_annotation_group -plot $plot -id $group_id]

  set_charts_property -annotation $arc_group -name layer.drawLayer -value MOUSE_OVER

  # angle1 angle2 innerRadius outerRadius label ind keyLabel exploded
  # value id rect tipId visible selected inside selectable editable clickable

  set a1 [get_charts_property -plot $plot -object $obj -name angle1]
  set a2 [get_charts_property -plot $plot -object $obj -name angle2]

  set ri [get_charts_property -plot $plot -object $obj -name innerRadius]

  set arc_id ${obj}_arc_annotation
  set tip    "$obj annotation"

  set da [expr {$a2 - $a1}]

  set arc [create_charts_pie_slice_annotation -plot $plot -position {0 0} -id $arc_id -tip $tip \
   -inner_radius 0.1 -outer_radius $ri -start_angle $a1 -span_angle $da -group $arc_group]

  set color "palette 0.5"

  set_charts_property -annotation $arc -name fill.color -value $color

  set text_id ${obj}_text_annotation

  set text [create_charts_text_annotation -plot $plot -id $text_id \
    -position {0 0} -text "Pie" -group $arc_group]

  set_charts_property -annotation $text -name objRef -value [list $arc center]

  return $arc_group
}

proc updateAnnotations { view plot } {
  disconnect_charts_signal -plot $plot -from plotObjsAdded -to updateAnnotations

  set objs [get_charts_data -plot $plot -name objects]
  set obj  [lindex $objs 1]

  set arc_group [create_arc_group $plot $obj]

  set_charts_property -annotation $arc_group -name mouseObjRef -value $obj
}
