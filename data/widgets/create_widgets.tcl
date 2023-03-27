set sets {qt CQ CQCharts}

foreach set $sets {
  set frame [qt_create_widget -type QFrame -name ${set}_frame]

  set layout [qt_create_layout -type QVBoxLayout -parent $frame]

  set tab [qt_create_widget -type QTabWidget -name ${set}_tab]

  set tab [qt_add_child_widget -parent $frame -child $tab]

  #set types [qt_get_widget_types -set $set -simple]
  set types [qt_get_widget_types -set $set]

  echo $types

  foreach type $types {
    set type1 $type
    #set type1 [regexp -inline -- CQCharts.* $type]

    if {$type1 != ""} {
      set frame1 [qt_create_widget -type QFrame -name frame1]

      set layout1 [qt_create_layout -type QVBoxLayout -parent $frame1]

      set frame1 [qt_add_child_widget -parent $tab -child $frame1 -label $type]

      set w [qt_create_widget -type $type -name $type -set $set]

      qt_add_child_widget -parent $frame1 -child $w
      qt_add_child_widget -parent $frame1 -stretch 1
    }
  }

  qt_set_property -object $frame -property visible -value 1
}
