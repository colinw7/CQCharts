set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set rect1 [create_charts_rectangle_annotation -plot $plotId -rectangle {{10 10} {20 20}}]

set_charts_property -annotation $rect1 -name shapeType -value CIRCLE

set text [create_charts_text_annotation -plot $plotId -position {4 0 px} -text "Hello"]

set_charts_property -annotation $text -name objRef -value [list $rect1 right]
