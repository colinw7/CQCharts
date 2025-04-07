set plot [create_charts_plot -type empty -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set r1 [create_charts_rectangle_annotation -plot $plot -id r1 -rectangle {10 10 20 20}]
set r2 [create_charts_rectangle_annotation -plot $plot -id r2 -rectangle {30 30 40 40}]

set startPos1 "@(${r1}) -1 5"
set endPos1   "@(${r2})  1 5"

set a1 [create_charts_arrow_annotation -plot $plot -start $startPos1 -end $endPos1]

set_charts_property -annotation $a1 -name fill.visible     -value 1
set_charts_property -annotation $a1 -name stroke.visible   -value 1
set_charts_property -annotation $a1 -name line.rectilinear -value 1
set_charts_property -annotation $a1 -name autoPath         -value 1
set_charts_property -annotation $a1 -name startAngle       -value 90
set_charts_property -annotation $a1 -name endAngle         -value 90

set r3 [create_charts_rectangle_annotation -plot $plot -id r3 -rectangle {50 50 60 60}]
set r4 [create_charts_rectangle_annotation -plot $plot -id r4 -rectangle {70 70 80 80}]

set startPos2 "@(${r3})  5 -1"
set endPos2   "@(${r4}) -5  1"

set a2 [create_charts_arrow_annotation -plot $plot -start $startPos2 -end $endPos2]

set_charts_property -annotation $a2 -name fill.visible     -value 1
set_charts_property -annotation $a2 -name stroke.visible   -value 1
set_charts_property -annotation $a2 -name line.rectilinear -value 1
set_charts_property -annotation $a2 -name autoPath         -value 1
set_charts_property -annotation $a2 -name startAngle       -value 0
set_charts_property -annotation $a2 -name endAngle         -value 180

set r5 [create_charts_rectangle_annotation -plot $plot -id r5 -rectangle {10 60 20 70}]
set r6 [create_charts_rectangle_annotation -plot $plot -id r6 -rectangle {30 80 40 90}]

set startPos3 "@(${r5}) 5 -1"
set endPos3   "@(${r6}) 1 -5"

set a3 [create_charts_arrow_annotation -plot $plot -start $startPos3 -end $endPos3]

set_charts_property -annotation $a3 -name fill.visible     -value 1
set_charts_property -annotation $a3 -name stroke.visible   -value 1
set_charts_property -annotation $a3 -name line.rectilinear -value 1
set_charts_property -annotation $a3 -name autoPath         -value 1
set_charts_property -annotation $a3 -name startAngle       -value 0
set_charts_property -annotation $a3 -name endAngle         -value 270
