set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_shape_annotation -plot $plotId -rectangle {{10 10} {20 20}}]
set rect2 [create_charts_shape_annotation -plot $plotId -rectangle {{80 80} {90 90}}]

set_charts_property -annotation $rect1 -name shapeType -value POLYGON
set_charts_property -annotation $rect1 -name numSides -value 7
set_charts_property -annotation $rect2 -name shapeType -value CIRCLE

set arrowId [create_charts_arrow_annotation -plot $plotId -start {0 0 px} -end {1 1 px}]

set_charts_property -annotation $arrowId -name startObjRef -value [list $rect1 intersect]
set_charts_property -annotation $arrowId -name endObjRef   -value [list $rect2 intersect]

set_charts_property -annotation $arrowId -name line.rectilinear -value 1
set_charts_property -annotation $arrowId -name line.width -value 16px
set_charts_property -annotation $arrowId -name frontHead.type -value TRIANGLE
set_charts_property -annotation $arrowId -name frontHead.length -value 32px
set_charts_property -annotation $arrowId -name tailHead.type -value TRIANGLE
set_charts_property -annotation $arrowId -name tailHead.length -value 32px
#set_charts_property -annotation $arrowId -name debugLabels -value 1
