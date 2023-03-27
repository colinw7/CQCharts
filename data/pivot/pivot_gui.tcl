if {0} {
set df {{
"A": ["foo", "foo", "foo", "foo", "foo", "bar", "bar", "bar", "bar"],
"B": ["one", "one", "one", "two", "two", "one", "one", "two", "two"],
"C": ["small", "large", "large", "small", "small", "large", "small", "small", "large"],
"D": [1, 2, 2, 3, 3, 4, 5, 6, 7],
"E": [2, 4, 5, 5, 6, 6, 8, 9, 9]}}

set model [load_charts_model -json @df]
#write_charts_model -model $model
} else {
set model [load_charts_model -csv skorea_covid/TimeAge.csv -first_line_header]
}

#---

set plot [create_charts_plot -type empty]

#---

set frame  [qt_create_widget -type QFrame -name frame]
set layout [qt_create_layout -type QVBoxLayout -parent $frame]

proc addColumnsEdit { labelStr name } {
  set hframe  [qt_create_widget -type QFrame -name ${name}_frame]
  set hlayout [qt_create_layout -type QHBoxLayout -parent $hframe]

  set label [qt_create_widget -type QLabel -name ${name}_label]
  set edit  [qt_create_widget -type CQCharts:ColumnsEdit -name ${name}_edit]

  qt_set_property -object $label -property text -value $labelStr
  qt_set_property -object $edit  -property basic -value 1

  qt_add_child_widget -parent $hframe -child $label
  qt_add_child_widget -parent $hframe -child $edit

  qt_add_child_widget -parent $::frame -child $hframe

  return $edit
}

proc addLineEdit { labelStr name } {
  set hframe  [qt_create_widget -type QFrame -name ${name}_frame]
  set hlayout [qt_create_layout -type QHBoxLayout -parent $hframe]

  set label [qt_create_widget -type QLabel -name ${name}_label]
  set edit  [qt_create_widget -type QLineEdit -name ${name}_edit]

  qt_set_property -object $label -property text -value $labelStr

  qt_add_child_widget -parent $hframe -child $label
  qt_add_child_widget -parent $hframe -child $edit

  qt_add_child_widget -parent $::frame -child $hframe

  return $edit
}

proc addControlsButtons { } {
  set hframe  [qt_create_widget -type QFrame -name buttonsFrame]
  set hlayout [qt_create_layout -type QHBoxLayout -parent $hframe]

  set button [qt_create_widget -type QPushButton -name okButton]

  qt_connect_widget -name $button -signal "clicked()" -proc execPivot

  qt_set_property -object $button -property text -value "OK"

  qt_add_child_widget -parent $hframe -child $button
  qt_add_child_widget -parent $hframe -stretch 1

  qt_add_child_widget -parent $::frame -child $hframe

  return $button
}

proc execPivot { } {
  #echo "execPivot"

  set hcolumns [qt_get_property -object $::hcolumnsEdit -property columns]
  set vcolumns [qt_get_property -object $::vcolumnsEdit -property columns]
  set dcolumns [qt_get_property -object $::dcolumnsEdit -property columns]

  set cmd [list create_charts_pivot_model -model $::model]

  if {$hcolumns != ""} {
    lappend cmd "-hcolumns"
    lappend cmd $hcolumns
  }

  if {$vcolumns != ""} {
    lappend cmd "-vcolumns"
    lappend cmd $vcolumns
  }

  if {$dcolumns != ""} {
    lappend cmd "-dcolumns"
    lappend cmd $dcolumns
  }

  set fillValue  [qt_get_property -object $::fillValueEdit  -property text]
  set valueTypes [qt_get_property -object $::valueTypesEdit -property text]

  if {$fillValue != ""} {
    lappend cmd "-fill_value"
    lappend cmd $fillValue
  }

  if {$valueTypes != ""} {
    lappend cmd "-value_types"
    lappend cmd $valueTypes
  }

  set pivotModel [eval $cmd]
  write_charts_model -model $pivotModel

  set pivotInd [get_charts_data -model $pivotModel -name ind]
  #echo $pivotInd

  qt_set_property -object $::pivotModelView -property modelInd -value $pivotInd
}

addColumnsEdit "H Columns" hcolumnsEdit
addColumnsEdit "V Columns" vcolumnsEdit
addColumnsEdit "D Columns" dcolumnsEdit

addLineEdit "Fill Value"  fillValueEdit
addLineEdit "Value Types" valueTypesEdit

qt_add_child_widget -parent $frame -stretch 1

addControlsButtons

#---

set ann [create_charts_widget_annotation -plot $plot -id fram \
  -rectangle [list 1 50 99 99 V] -widget $frame]

set frame [get_charts_data -annotation $ann -name widget_path]
#echo $frame

proc editWidgetName { name } {
  return $::frame|${name}_frame|${name}_edit
}

set hcolumnsEdit [editWidgetName hcolumnsEdit]
#echo "$hcolumnsEdit"
set vcolumnsEdit [editWidgetName vcolumnsEdit]
#echo "$vcolumnsEdit"
set dcolumnsEdit [editWidgetName dcolumnsEdit]
#echo "$dcolumnsEdit"

set fillValueEdit  [editWidgetName fillValueEdit]
set valueTypesEdit [editWidgetName valueTypesEdit]

#---

set modelView [qt_create_widget -type CQCharts:ModelViewHolder -name modelView]

set modelViewAnn [create_charts_widget_annotation -plot $plot -id modelView \
  -rectangle [list 1 1 49 49 V] -widget $modelView]

set modelView [get_charts_data -annotation $modelViewAnn -name widget_path]
#echo $modelView

#---

set pivotModelView [qt_create_widget -type CQCharts:ModelViewHolder -name pivotModelView]

set pivotModelViewAnn [create_charts_widget_annotation -plot $plot -id pivotModelView \
  -rectangle [list 50 1 99 49 V] -widget $pivotModelView]

set pivotModelView [get_charts_data -annotation $pivotModelViewAnn -name widget_path]
#echo $pivotModelView
