proc modelSelect { } {
  set model [qt_get_property -object $::modelTable -property selectedModel]

  echo "$::modelTable $model"

  qt_set_property -object $::modelView     -property modelInd -value $model
  qt_set_property -object $::detailsTable  -property modelInd -value $model
  qt_set_property -object $::columnControl -property modelInd -value $model
}

proc columnSelect { c } {
  qt_set_property -object $::columnControl -property columnData -value $c
}

cd skorea_covid

#set model1  [load_charts_model -csv Case.csv -first_line_header]
#set model2  [load_charts_model -csv PatientInfo.csv -first_line_header]
#set model3  [load_charts_model -csv Policy.csv -first_line_header]
#set model4  [load_charts_model -csv Region.csv -first_line_header]
#set model5  [load_charts_model -csv SearchTrend.csv -first_line_header]
#set model6  [load_charts_model -csv SeoulFloating.csv -first_line_header]
#set model7  [load_charts_model -csv Time.csv -first_line_header]
set timeAgeModel      [load_charts_model -csv TimeAge.csv -first_line_header]
set timeGenderModel   [load_charts_model -csv TimeGender.csv -first_line_header]
set timeProvinceModel [load_charts_model -csv TimeProvince.csv -first_line_header]
#set model11 [load_charts_model -csv Weather.csv -first_line_header]

set_charts_data -model $timeAgeModel -column 0 -name column_type \
  -value {{time} {format %Y-%m-%d} {oformat %F}}
set_charts_data -model $timeGenderModel -column 0 -name column_type \
  -value {{time} {format %Y-%m-%d} {oformat %F}}
set_charts_data -model $timeGenderModel -column 0 -name column_type \
  -value {{time} {format %Y-%m-%d} {oformat %F}}

set timeAgePivot [create_charts_pivot_model -model $timeAgeModel \
  -vcolumns date -hcolumns age -dcolumns {confirmed deceased}]
set timeGenderPivot [create_charts_pivot_model -model $timeGenderModel \
  -vcolumns date -hcolumns sex -dcolumns {confirmed deceased}]
set provincePivot [create_charts_pivot_model -model $timeProvinceModel \
  -vcolumns date -hcolumns province -dcolumns {confirmed deceased}]

set plot [create_charts_plot -type empty]

#---

set modelTable [qt_create_widget -type CQChartsModelTable -name modelTable]
echo $modelTable

qt_connect_widget -name $modelTable -signal "itemSelectionChanged()" -proc modelSelect

set modelTableAnn [create_charts_widget_annotation -plot $plot -id modelTable \
  -rectangle [list 1 1 20 99 V] -widget $modelTable]

set modelTable [get_charts_data -annotation $modelTableAnn -name widget_path]

#---

set modelView [qt_create_widget -type CQChartsModelViewHolder -name modelView]
echo $modelView

qt_connect_widget -name $modelView -signal "columnClicked(int)" -proc columnSelect

set modelViewAnn [create_charts_widget_annotation -plot $plot -id modelView \
  -rectangle [list 21 1 99 49 V] -widget $modelView]

set modelView [get_charts_data -annotation $modelViewAnn -name widget_path]

#---

set detailsTable [qt_create_widget -type CQChartsModelDetailsTable -name detailsTable]
echo $detailsTable

set detailsTableAnn [create_charts_widget_annotation -plot $plot -id detailsTable \
  -rectangle [list 21 50 75 99 V] -widget $detailsTable]

set detailsTable [get_charts_data -annotation $detailsTableAnn -name widget_path]

#---

set columnControl [qt_create_widget -type CQChartsModelColumnDataControl -name columnControl]
echo $columnControl

set columnControlAnn [create_charts_widget_annotation -plot $plot -id columnControl \
  -rectangle [list 76 51 99 99 V] -widget $columnControl]

set columnControl [get_charts_data -annotation $columnControlAnn -name widget_path]
