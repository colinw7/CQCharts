proc radioChanged { args } {
  set rc [qt_get_property -object $::plot1Radio -property checked]

  set_charts_property -plot $::plot1 -name visible -value $rc
  set_charts_property -plot $::plot2 -name visible -value [expr {1 - $rc}]
}

#---

set model1 [load_charts_model -csv data/big_ints.csv -first_line_header]
set model2 [load_charts_model -csv data/big_ints.csv -first_line_header \
 -column_type {{{1 real} {format %gM} {format_scale 0.000001}}}]

set plot1 [create_charts_plot -type distribution -model $model1 -columns {{values 1}}]
set plot2 [create_charts_plot -type distribution -model $model2 -columns {{values 1}}]

set_charts_property -plot $plot1 -name visible -value 1
set_charts_property -plot $plot2 -name visible -value 0

set view [get_charts_data -plot $plot1 -name view]

place_charts_plots -horizontal -view $view [list $plot1 $plot2]

#---

# add control frame
set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

set plot1Radio [qt_create_widget -type QRadioButton -name plot1Radio]
set plot2Radio [qt_create_widget -type QRadioButton -name plot2Radio]

qt_set_property -object $plot1Radio -property text -value "Plot 1"
qt_set_property -object $plot2Radio -property text -value "Plot 2"

qt_set_property -object $plot1Radio -property checked -value 1

qt_connect_widget -name $plot1Radio -signal "toggled(bool)" -proc radioChanged
qt_connect_widget -name $plot2Radio -signal "toggled(bool)" -proc radioChanged

qt_add_child_widget -parent $frame -child $plot1Radio
qt_add_child_widget -parent $frame -child $plot2Radio

#---

set widgetAnn [create_charts_widget_annotation -view $view \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $widgetAnn -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -view $view -name view_path]

set plot1Radio "$view_path|frame|$plot1Radio"
set plot2Radio "$view_path|frame|$plot2Radio"
