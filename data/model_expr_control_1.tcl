proc columnSelect { c } {
  qt_set_property -object $::modelExprControl -property column -value $c
}

proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

#---

# load model
set model [load_charts_model -csv data/chord-cities.csv -comment_header]

# create empty plot
set plot [create_charts_plot -model $model -type empty]

#---

set modelView [qt_create_widget -type CQChartsModelViewHolder -name modelView]

qt_connect_widget -name $modelView -signal "columnClicked(int)" -proc columnSelect

set modelViewAnn [create_charts_widget_annotation -plot $plot -id modelView \
  -rectangle [list 1 1 99 49 V] -widget $modelView]

set modelView [get_charts_data -annotation $modelViewAnn -name widget_path]

#---

set modelExprControl [qt_create_widget -type CQChartsModelExprControl -name modelExprControl]

set modelExprControlAnn [create_charts_widget_annotation -plot $plot -id modelExprControl \
  -rectangle [list 1 50 99 99 V] -widget $modelExprControl]

set modelExprControl [get_charts_data -annotation $modelExprControl -name widget_path]

#---

connect_charts_signal -plot $plot -from annotationPressed -to annotationSlot
