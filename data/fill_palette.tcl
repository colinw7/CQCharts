set paletteName  moreland
set paletteXMin  0.2
set paletteXMax  0.4
set paletteAngle 45

proc updatePattern { } {
  set_charts_property -annotation $::rect -name fill.pattern \
    -value "PALETTE:${::paletteName}\[${::paletteXMin},${::paletteXMax}\]@${::paletteAngle}"
}

proc paletteNameChanged { } {
  set ::paletteName [qt_get_property -object $::paletteEdit -property text]

  echo "paletteNameChanged $::paletteName"

  updatePattern
}

proc angleChanged { a } {
  echo "angleChanged $a"

  set ::paletteAngle $a

  updatePattern
}

set plot [create_charts_plot -type empty -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set rect [create_charts_rectangle_annotation -plot $plot -id rect \
  -start {20 20} -end {80 90}]

updatePattern

#---

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QHBoxLayout -parent $frame]

set paletteEdit [qt_create_widget -type QLineEdit -name paletteEdit]
set angleEdit   [qt_create_widget -type CQIntegerSpin -name angleEdit]

qt_set_property -object $paletteEdit -property text  -value $paletteName
qt_set_property -object $angleEdit   -property value -value $paletteAngle

qt_connect_widget -name $paletteEdit -signal "editingFinished()" \
  -proc paletteNameChanged
qt_connect_widget -name $angleEdit -signal "valueChanged(int)" \
  -proc angleChanged

qt_add_child_widget -parent $frame -child $paletteEdit
qt_add_child_widget -parent $frame -child $angleEdit

set annotation [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set paletteEdit "$view_path|frame|$paletteEdit"
set angleEdit   "$view_path|frame|$angleEdit"
