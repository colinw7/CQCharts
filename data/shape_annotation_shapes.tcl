set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set shape1 [create_charts_shape_annotation -plot $plotId -rectangle {{10 10} {20 20}}]
set shape2 [create_charts_shape_annotation -plot $plotId -rectangle {{20 20} {30 30}}]
set shape3 [create_charts_shape_annotation -plot $plotId -rectangle {{30 30} {40 40}}]
set shape4 [create_charts_shape_annotation -plot $plotId -rectangle {{40 40} {50 50}}]
set shape5 [create_charts_shape_annotation -plot $plotId -rectangle {{50 50} {60 60}}]
set shape6 [create_charts_shape_annotation -plot $plotId -rectangle {{60 60} {70 70}}]
set shape7 [create_charts_shape_annotation -plot $plotId -rectangle {{70 70} {80 80}}]

set_charts_property -annotation $shape1 -name shapeType -value TRIANGLE
set_charts_property -annotation $shape2 -name shapeType -value DIAMOND
set_charts_property -annotation $shape3 -name shapeType -value BOX
set_charts_property -annotation $shape4 -name shapeType -value POLYGON
set_charts_property -annotation $shape4 -name shapeType -value CIRCLE
set_charts_property -annotation $shape5 -name shapeType -value DOUBLE_CIRCLE
set_charts_property -annotation $shape6 -name shapeType -value DOT_LINE
