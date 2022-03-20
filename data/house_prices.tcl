proc modelSelect { } {
  set model [qt_get_property -object $::modelTable -property selectedModel]

  echo "$::modelTable $model"

  qt_set_property -object $::modelView    -property modelInd -value $model
  qt_set_property -object $::detailsTable -property modelInd -value $model
}

set model [load_charts_model -csv data/HouseData.csv -first_line_header]

set plot [create_charts_plot -type empty]

#---

set modelTable [qt_create_widget -type CQChartsModelTable -name modelTable]
echo $modelTable

qt_connect_widget -name $modelTable -signal "itemSelectionChanged()" -proc modelSelect

set modelTableAnn [create_charts_widget_annotation -plot $plot -id modelTable \
  -rectangle [list 2 2 20 98 V] -widget $modelTable]

set modelTable [get_charts_data -annotation $modelTableAnn -name widget_path]

#---

set modelView [qt_create_widget -type CQChartsModelViewHolder -name modelView]
echo $modelView

set modelViewAnn [create_charts_widget_annotation -plot $plot -id modelView \
  -rectangle [list 24 2 98 49 V] -widget $modelView]

set modelView [get_charts_data -annotation $modelViewAnn -name widget_path]

#---

set detailsTable [qt_create_widget -type CQChartsModelDetailsTable -name detailsTable]
echo $detailsTable

set detailsTableAnn [create_charts_widget_annotation -plot $plot -id detailsTable \
  -rectangle [list 24 51 98 98 V] -widget $detailsTable]

set detailsTable [get_charts_data -annotation $detailsTableAnn -name widget_path]
