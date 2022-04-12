# create empty plot
set plot [create_charts_plot -type empty]

#---

set viewLayerTable [qt_create_widget -type CQChartsViewLayerTable -name viewLayerTable]

set symbolSetsListAnn [create_charts_widget_annotation -plot $plot -id viewLayerTable \
  -rectangle [list 1 1 50 99 V] -widget $viewLayerTable]

set viewLayerTable [get_charts_data -annotation $viewLayerTable -name widget_path]

#---

set plotLayerTable [qt_create_widget -type CQChartsPlotLayerTable -name plotLayerTable]

set symbolSetsListAnn [create_charts_widget_annotation -plot $plot -id plotLayerTable \
  -rectangle [list 51 1 99 99 V] -widget $plotLayerTable]

set plotLayerTable [get_charts_data -annotation $plotLayerTable -name widget_path]
