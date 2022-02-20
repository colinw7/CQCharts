set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set shape1 [create_charts_shape_annotation -plot $plotId -rectangle {{10 10} {20 20}}]

set_charts_property -annotation $shape1 -name shapeType -value CIRCLE

set shape [create_charts_shape_annotation -plot $plotId -start {4 -8 px} -end {20 8 px}]

set_charts_property -annotation $shape -name objRef -value [list $shape1 right]
