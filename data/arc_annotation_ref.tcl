set modelId [load_charts_model -csv data/arrowstyle.csv]

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set point1 [create_charts_point_annotation -plot $plot -position {10 10}]
set point2 [create_charts_point_annotation -plot $plot -position {90 90}]

set rect1 [create_charts_rectangle_annotation -plot $plot -rectangle {{20 40} {40 60}}]

set_charts_property -annotation $rect1 -name shapeType -value CIRCLE

set arc1 [create_charts_arc_annotation -plot $plot \
 -start {{0 0} {1 1}} -end {{0 0} {1 1}}]
set arc2 [create_charts_arc_annotation -plot $plot \
 -start {{0 0} {1 1}} -end {{0 0} {1 1}}]

set_charts_property -annotation $arc1 -name startObjRef -value [list $point1 intersect]
set_charts_property -annotation $arc1 -name endObjRef   -value [list $point2 intersect]
set_charts_property -annotation $arc1 -name frontType   -value ARROW
set_charts_property -annotation $arc1 -name tailType    -value ARROW

set_charts_property -annotation $arc2 -name startObjRef -value [list $rect1 intersect]
set_charts_property -annotation $arc2 -name endObjRef   -value [list $rect1 intersect]
set_charts_property -annotation $arc2 -name frontType   -value ARROW
set_charts_property -annotation $arc2 -name tailType    -value ARROW

