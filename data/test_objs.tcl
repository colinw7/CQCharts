
set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY Plot"]

set objs { }

set key_obj   [get_charts_data -plot $plot -name key_obj]
set xaxis_obj [get_charts_data -plot $plot -name xaxis_obj]
set yaxis_obj [get_charts_data -plot $plot -name yaxis_obj]
set title_obj [get_charts_data -plot $plot -name title_obj]

lappend objs $key_obj
lappend objs $xaxis_obj
lappend objs $yaxis_obj
lappend objs $title_obj

echo "Key: $key_obj"
echo "X Axis: $xaxis_obj"
echo "Y Axis: $yaxis_obj"
echo "Title: $title_obj"

proc show_rects { } {
  foreach obj $::objs {
    set rect [get_charts_property -plot $::plot -object $obj -name rect]

    echo "$obj $rect"
  }

  set plotObjs [get_charts_data -plot $::plot -name objects]

  foreach obj $plotObjs {
    set rect [get_charts_property -plot $::plot -object $obj -name rect]

    echo "$obj $rect"
  }
}

#qt_timer -delay 1000 -proc show_rects

#---

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QHBoxLayout -parent $frame]

set rectButton [qt_create_widget -type QPushButton -name rectButton]

qt_set_property -object $rectButton -property text -value "Rects"

qt_connect_widget -name $rectButton -signal "clicked()" -proc show_rects

qt_add_child_widget -parent $frame -child $rectButton

set annotation [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set rectButton "$view_path|frame|$rectButton"
