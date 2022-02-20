set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_shape_annotation -plot $plotId -rectangle {{10 10} {20 20}}]

set_charts_property -annotation $rect1 -name shapeType -value CIRCLE

set checkbox [qt_create_widget -type QCheckBox -name check]

qt_set_property -object $checkbox -property text -value "Check"

set widget [create_charts_widget_annotation -plot $plotId \
  -position {4 -16 px} -widget $checkbox]

set_charts_property -annotation $widget -name objRef -value [list $rect1 right]
