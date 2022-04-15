# create empty plot
set plot [create_charts_plot -type empty]

#---

if {0} {
set viewLayerTable [qt_create_widget -type CQChartsViewLayerTable -name viewLayerTable]

set viewLayerTableAnn [create_charts_widget_annotation -plot $plot -id viewLayerTable \
  -rectangle [list 1 1 50 99 V] -widget $viewLayerTable]

set viewLayerTable [get_charts_data -annotation $viewLayerTableAnn -name widget_path]

#---

set plotLayerTable [qt_create_widget -type CQChartsPlotLayerTable -name plotLayerTable]

set plotLayerTableAnn [create_charts_widget_annotation -plot $plot -id plotLayerTable \
  -rectangle [list 51 1 99 99 V] -widget $plotLayerTable]

set plotLayerTable [get_charts_data -annotation $plotLayerTableAnn -name widget_path]
} else {
set layerTableControl [qt_create_widget -type CQChartsLayerTableControl -name plotLayerTable]

set layerTableControlAnn [create_charts_widget_annotation -plot $plot -id layerTableControl \
  -rectangle [list 1 1 99 99 V] -widget $layerTableControl]

set layerTableControl [get_charts_data -annotation $layerTableControlAnn -name widget_path]
}
