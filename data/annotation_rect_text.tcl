set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

proc create_rect { plot name } {
  set rect [create_charts_rectangle_annotation -plot $plot -start {10 10} -end {90 90}]
  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $rect -name textInd -value $text
  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}

  return $rect
}

set rect [create_rect $plot "Text inside Rectangle"]
