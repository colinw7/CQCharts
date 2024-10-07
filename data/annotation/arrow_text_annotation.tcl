set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plot -rectangle {10 10 20 20}]
set rect2 [create_charts_rectangle_annotation -plot $plot -rectangle {80 10 90 20}]
set rect3 [create_charts_rectangle_annotation -plot $plot -rectangle {40 80 60 90}]

set connectPos1 "@($rect1 intersect) 0 0"
set connectPos2 "@($rect2 intersect) 0 0"
set connectPos3 "@($rect3 intersect) 0 0"

set arrow1 [create_charts_arrow_annotation -plot $plot -start $connectPos1 -end $connectPos2]
set arrow2 [create_charts_arrow_annotation -plot $plot -start $connectPos2 -end $connectPos3]
set arrow3 [create_charts_arrow_annotation -plot $plot -start $connectPos3 -end $connectPos1]

set text1 [create_charts_text_annotation -plot $plot -text One -position {0 0}]
set text2 [create_charts_text_annotation -plot $plot -text Two -position {0 0}]
set text3 [create_charts_text_annotation -plot $plot -text Three -position {0 0}]

set_charts_property -annotation $arrow1 -name line.width -value 4px
set_charts_property -annotation $arrow1 -name fill.color -value red
set_charts_property -annotation $arrow1 -name textInd -value $text1

set_charts_property -annotation $arrow2 -name line.width -value 6px
set_charts_property -annotation $arrow2 -name fill.color -value green
set_charts_property -annotation $arrow2 -name textInd -value $text2

set_charts_property -annotation $arrow3 -name line.width -value 8px
set_charts_property -annotation $arrow3 -name fill.color -value blue
set_charts_property -annotation $arrow3 -name textInd -value $text3
