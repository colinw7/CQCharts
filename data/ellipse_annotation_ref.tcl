set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plotId -rectangle {{10 10} {20 20}}]

set_charts_property -annotation $rect1 -name shapeType -value CIRCLE

set ellipse [create_charts_ellipse_annotation -plot $plotId -center {4 0 px} -rx 1 -ry 1]

set_charts_property -annotation $ellipse -name objRef -value [list $rect1 right]
