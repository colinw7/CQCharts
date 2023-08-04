set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plotId -rectangle {{10 10} {20 20}}]

set rect [create_charts_rectangle_annotation -plot $plotId -start {8 -32 px} -end {40 32 px}]

set_charts_property -annotation $rect -name objRef -value [list $rect1 right]
