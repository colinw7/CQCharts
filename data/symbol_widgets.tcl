proc symbolSetsListNameChanged { name } {
  echo $name
  qt_set_property -object $::symbolsList -property setName -value $name
}

proc symbolsListSymbolChanged { } {
  set symbol [qt_get_property -object $::symbolsList -property symbol]
  echo $symbol

  qt_set_property -object $::symbolEditor -property symbol -value $symbol
}

# create empty plot
set plot [create_charts_plot -type empty]

#---

set symbolSetsList [qt_create_widget -type CQChartsSymbolSetsList -name symbolSetsList]

set symbolSetsListAnn [create_charts_widget_annotation -plot $plot -id symbolSetsList \
  -rectangle [list 1 1 99 33 V] -widget $symbolSetsList]

set symbolSetsList [get_charts_data -annotation $symbolSetsList -name widget_path]

qt_connect_widget -name $symbolSetsList -signal "nameChanged(const QString &)" \
                  -proc symbolSetsListNameChanged

#---

#set symbolsList [qt_create_widget -type CQChartsSymbolsList -name symbolsList]
set symbolsList [qt_create_widget -type CQChartsSymbolsListControl -name symbolsList]

set symbolsListAnn [create_charts_widget_annotation -plot $plot -id symbolsList \
  -rectangle [list 1 34 99 66 V] -widget $symbolsList]

set symbolsList [get_charts_data -annotation $symbolsList -name widget_path]

qt_connect_widget -name $symbolsList -signal "symbolChanged()" \
                  -proc symbolsListSymbolChanged

#---

set symbolEditor [qt_create_widget -type CQChartsSymbolEditor -name symbolEditor]

set symbolEditorAnn [create_charts_widget_annotation -plot $plot -id symbolEditor \
  -rectangle [list 1 67 99 99 V] -widget $symbolEditor]

set symbolEditor [get_charts_data -annotation $symbolEditor -name widget_path]
