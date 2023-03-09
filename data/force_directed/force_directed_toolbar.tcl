proc viewResized { view } {
  #echo "viewResized"

  set pos1 [get_charts_data -view $view -name pixel_position -data {0 0 V}]
  set pos2 [get_charts_data -view $view -name pixel_position -data {100 100 V}]
  #echo "$pos1 $pos2"

  set w [expr {[lindex $pos2 0] - [lindex $pos1 0]}]
  set h [expr {[lindex $pos2 1] - [lindex $pos1 1]}]
  #echo "$w $h"

  set_charts_property -annotation $::tbarAnn -name rectangle -value [list 0 0 $w 64 px]
}

proc animateStateChanged { view plot b } {
  echo "animateStateChanged $view $plot $b"

  set animateCheck "$::tbar|animateCheck"

  set checked [qt_get_property -object $animateCheck -property checked]

  if {$b != $checked} {
    qt_set_property -object $animateCheck -property checked -value $b
  }
}

proc animateCheckClicked { } {
  #echo "animateCheckClicked"

  set running [get_charts_property -plot $::plot -name animation.running]

  set_charts_property -plot $::plot -name animation.running -value [expr {1 - $running}]
}

proc resetClicked { } {
  echo "resetClicked"

  set_charts_property -plot $::plot -name placement.reset -value 1
}

set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{link 0} {value 1}}]

set_charts_property -plot $plot -name title.visible -value 0

set_charts_property -plot $::plot -name margins.fit.left   -value 0
set_charts_property -plot $::plot -name margins.fit.right  -value 0
set_charts_property -plot $::plot -name margins.fit.top    -value "64 px"
set_charts_property -plot $::plot -name margins.fit.bottom -value 0

set_charts_property -plot $::plot -name margins.pixelMargin -value 0

set_charts_property -plot $::plot -name animation.showBusyButton -value 0

set tbar   [qt_create_widget -type QFrame -name tbar]
set layout [qt_create_layout -type QHBoxLayout -parent $tbar -name layout]

qt_set_property -object $layout -property margin -value 0
qt_set_property -object $layout -property spacing -value 2

set label  [qt_create_widget -type QLabel -name label]
qt_set_property -object $label -property text -value "Force Directed Budget"
set animateCheck [qt_create_widget -type QCheckBox -name animateCheck]
qt_set_property -object $animateCheck -property text -value "Animate"
set resetButton [qt_create_widget -type QPushButton -name resetButton]
qt_set_property -object $resetButton -property text -value "Reset"

qt_connect_widget -name $animateCheck -signal "clicked()" -proc animateCheckClicked
qt_connect_widget -name $resetButton -signal "clicked()" -proc resetClicked

qt_add_child_widget -parent $tbar -child $label
qt_add_child_widget -parent $tbar -child $animateCheck
qt_add_child_widget -parent $tbar -child $resetButton
qt_add_stretch -parent $tbar

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name theme.dark -value 1

set tbarAnn [create_charts_widget_annotation -view $view -id tbar -widget $tbar \
              -rectangle [list 0 0 100 10 V]]

set tbar [get_charts_data -annotation $tbarAnn -name widget_path]

connect_charts_signal -plot $plot -from animateStateChanged -to animateStateChanged

connect_charts_signal -view $view -from viewResized -to viewResized

animateStateChanged $view $plot 1

viewResized $view
