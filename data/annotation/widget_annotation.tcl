proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set pos [get_charts_property -annotation $id -name position]
  puts "pos: $pos"

  set widget [get_charts_property -annotation $id -name widget]
  puts "widget: $widget"
}

set modelId [load_charts_model -csv data/p2.csv]

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

set viewId [get_charts_property -plot $plot -name state.viewId]

set types [qt_create_widget -type ?]

set x -1
set y  1

proc can_create_widget { type } {
  if {$type == "QCheckBox"} { return 1 }
  if {$type == "QComboBox"} { return 1 }
  if {$type == "QLabel"} { return 1 }
# if {$type == "QLineEdit"} { return 1 }
  if {$type == "QRadioButton"} { return 1 }
# if {$type == "QSpinBox"} { return 1 }
# if {$type == "QTextEdit"} { return 1 }
  if {$type == "QPushButton"} { return 1 }
  if {$type == "QToolButton"} { return 1 }

  if {$type == "QColorDialog"} { return 0 }
  if {$type == "QDialog"} { return 0 }
  if {$type == "QDockWidget"} { return 0 }
  if {$type == "QFileDialog"} { return 0 }
  if {$type == "QFontDialog"} { return 0 }
  if {$type == "QMainWindow"} { return 0 }
  if {$type == "QMdiArea"} { return 0 }
  if {$type == "QMdiSubWindow"} { return 0 }
  if {$type == "QMenu"} { return 0 }
  if {$type == "QMenuBar"} { return 0 }
  if {$type == "QMessageBox"} { return 0 }
  if {$type == "QPrintDialog"} { return 0 }
  if {$type == "QProgressDialog"} { return 0 }
  if {$type == "QTableView"} { return 0 }
  if {$type == "QTreeView"} { return 0 }
  if {$type == "QUndoView"} { return 0 }
  if {$type == "QWizard"} { return 0 }
  if {$type == "QWizardPage"} { return 0 }

# return 1
  return 0
}

proc set_text_property { type w text } {
  if {[qt_has_property -object $w -property text -writable]} {
    qt_set_property -object $w -property text -value $text
  }
}

foreach type $types {
  if {! [can_create_widget $type]} {
    continue
  }

  echo "$type $x $y"

  set w [qt_create_widget -type $type -name $type]

  set_text_property $type $w $type

  set ann [create_charts_widget_annotation -plot $plot -id $type \
    -position [list $x $y] -widget $type]

  set y [expr {$y - 0.1}]

  if {$y >= 1.0} {
    set y 1

    set x [expr {$x + 0.1}]
  }
}

connect_charts_signal -plot $plot -from annotationPressed -to annotationSlot
