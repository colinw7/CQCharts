set layouts [qt_get_layout_types]

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

foreach layoutType $layouts {
  set subFrame [qt_create_widget -type QFrame -name ${layoutType}_frame]

  set layout [qt_create_layout -type $layoutType -parent $subFrame]

  set button1 [qt_create_widget -type QPushButton -name button]
  qt_set_property -object $button1 -property text -value "$layoutType 1"
  set w [qt_add_child_widget -parent $subFrame -child $button1 -row 0 -column 0 -label "One"]

  set button2 [qt_create_widget -type QPushButton -name button]
  qt_set_property -object $button2 -property text -value "$layoutType 2"
  set w [qt_add_child_widget -parent $subFrame -child $button2 -row 1 -column 1 -label "Two"]

  set w [qt_add_child_widget -parent $frame -child $subFrame]
}

qt_set_property -object $frame -property visible -value 1
