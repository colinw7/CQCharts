set modelId [load_charts_model -csv data/arrowstyle.csv]

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plot -rectangle {{20 40} {40 60}}]

set_charts_property -annotation $rect1 -name shapeType -value CIRCLE

set arc1 [create_charts_arc_annotation -plot $plot -start {0 0} -end {1 1}]

set_charts_property -annotation $arc1 -name startObjRef -value [list $rect1 intersect]
set_charts_property -annotation $arc1 -name endObjRef   -value [list $rect1 intersect]
set_charts_property -annotation $arc1 -name frontType   -value ARROW
set_charts_property -annotation $arc1 -name tailType    -value ARROW

