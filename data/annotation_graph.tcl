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

set rect1 [create_rect $plot $group "Rect1"]
set rect2 [create_rect $plot $group "Rect2"]
set rect3 [create_rect $plot $group "Rect3"]
set rect4 [create_rect $plot $group "Rect4"]
set rect5 [create_rect $plot $group "Rect5"]
set rect6 [create_rect $plot $group "Rect6"]

set arc1 [create_arc $plot $group $rect1 $rect4 10]
set arc2 [create_arc $plot $group $rect2 $rect4 20]
set arc3 [create_arc $plot $group $rect3 $rect4 30]
set arc4 [create_arc $plot $group $rect4 $rect5 40]
set arc5 [create_arc $plot $group $rect4 $rect6 50]

set_charts_property -annotation $group -name layout.type -value GRAPH
