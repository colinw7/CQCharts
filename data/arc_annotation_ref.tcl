set modelId [load_charts_model -csv data/arrowstyle.csv]

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plot -rectangle {{10 10} {20 20}}]
set rect2 [create_charts_rectangle_annotation -plot $plot -rectangle {{80 80} {90 90}}]

set arc [create_charts_arc_annotation -plot $plot \
 -start {{0 0} {1 1}} -end {{0 0} {1 1}}]

set_charts_property -annotation $arc -name startObjRef -value [list $rect1 intersect]
set_charts_property -annotation $arc -name endObjRef   -value [list $rect2 intersect]

