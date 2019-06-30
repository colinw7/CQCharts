set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations" \
 -ymax 100]

set types [create_charts_point_annotation -plot $plotId -position {0 0} -type ?]

set x1 0
set x2 100
set y -60

foreach type $types {
  create_charts_point_annotation -plot $plotId -position [list $x1 $y] -type $type -size 16px \
   -stroked 1 -filled 0
  create_charts_point_annotation -plot $plotId -position [list $x2 $y] -type $type -size 16px \
   -stroked 0 -filled 1 -fill_color "#4444aa" -fill_alpha 0.5

  incr y 10
}
