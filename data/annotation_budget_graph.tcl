set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set group [create_charts_annotation_group -plot $plot -id group]

proc create_rect { plot group name } {
  #echo "create_rect $plot $group $name"

  set rect [create_charts_rectangle_annotation -plot $plot -start {0 0} -end {10 10} -group $group]
  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $rect -name textInd -value $text
  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}

  return $rect
}

proc create_arc { plot group from to value } {
  #echo "create_arc $plot $group $from $to"

  set arc [create_charts_arc_annotation -plot $plot -start {0 0} -end {1 1} -group $group]

  set_charts_property -annotation $arc -name value -value $value

  set_charts_property -annotation $arc -name startObjRef -value [list $from intersect]
  set_charts_property -annotation $arc -name endObjRef   -value [list $to   intersect]

  set_charts_property -annotation $arc -name fill.alpha -value 0.3

  return $arc
}

set model [load_charts_model -csv data/sankey.csv -comment_header -column_type {{{0 name_pair}}}]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set nameNode {}

for {set r 0} {$r < $nr} {incr r} {
  set path  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set names [split $path "/"]

  set from [lindex $names 0]
  set to   [lindex $names 1]

  if {! [dict exists $nameNode $from]} {
    set rect [create_rect $plot $group $from]

    dict set nameNode $from $rect
  }

  if {! [dict exists $nameNode $to]} {
    set rect [create_rect $plot $group $to]

    dict set nameNode $to $rect
  }

  set fromNode [dict get $nameNode $from]
  set toNode   [dict get $nameNode $to]

  set arc [create_arc $plot $group $fromNode $toNode $value]
}

set_charts_property -annotation $group -name layout.type -value GRAPH
