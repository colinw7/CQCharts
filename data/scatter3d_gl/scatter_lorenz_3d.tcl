set model [create_charts_fractal_model -type lorenz]

set view [create_charts_view -3d]

set plot [create_charts_plot -view $view -model $model -type scatter3d \
  -columns {{x 0} {y 1} {z 2}} -title "Scatter (Lorenz)"]

#set expr_model [get_charts_data -model $model -name expr_model]
#set data_model [get_charts_data -model $model -name data_model]
set base_model [get_charts_data -model $model -name base_model]

###----

proc valueChanged { } {
  set oldA [qt_get_property -object $::base_model -property a]
  set oldB [qt_get_property -object $::base_model -property b]
  set oldC [qt_get_property -object $::base_model -property c]

  set a [qt_get_property -object $::aEdit -property text]
  set b [qt_get_property -object $::bEdit -property text]
  set c [qt_get_property -object $::cEdit -property text]

  if {$a != $oldA || $b != $oldB || $c != $oldC} {
    qt_set_property -object $::base_model -property a -value $a
    qt_set_property -object $::base_model -property b -value $b
    qt_set_property -object $::base_model -property c -value $c

    echo "valueChanged $a $b $c"
  }
}

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QHBoxLayout -parent $frame]

set aEdit [qt_create_widget -type QLineEdit -name aEdit]
set bEdit [qt_create_widget -type QLineEdit -name bEdit]
set cEdit [qt_create_widget -type QLineEdit -name cEdit]

qt_set_property -object $aEdit -property text -value 10.0
qt_set_property -object $bEdit -property text -value 28.0
qt_set_property -object $cEdit -property text -value [expr 8.0/3.0]

qt_connect_widget -name $aEdit -signal "editingFinished()" -proc valueChanged
qt_connect_widget -name $bEdit -signal "editingFinished()" -proc valueChanged
qt_connect_widget -name $cEdit -signal "editingFinished()" -proc valueChanged

qt_add_child_widget -parent $frame -child $aEdit
qt_add_child_widget -parent $frame -child $bEdit
qt_add_child_widget -parent $frame -child $cEdit

set annotation [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set aEdit "$view_path|glWidget|frame|$aEdit"
set bEdit "$view_path|glWidget|frame|$bEdit"
set cEdit "$view_path|glWidget|frame|$cEdit"
