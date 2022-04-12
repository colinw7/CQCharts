proc annotationSlot { args } {
  echo "annotationSlot $args"
}

# create empty plot
set plot [create_charts_plot -type empty]

#---

set colorsEditList [qt_create_widget -type CQColorsEditList -name colorsEditList]

set colorsEditListAnn [create_charts_widget_annotation -plot $plot -id colorsEditList \
  -rectangle [list 1 1 50 50 V] -widget $colorsEditList]

set colorsEditList [get_charts_data -annotation $colorsEditList -name widget_path]

#---

set paletteCanvas [qt_create_widget -type CQChartsPaletteCanvas -name paletteCanvas]

set paletteCanvasAnn [create_charts_widget_annotation -plot $plot -id paletteCanvas \
  -rectangle [list 1 51 50 99 V] -widget $paletteCanvas]

set paletteCanvas [get_charts_data -annotation $paletteCanvas -name widget_path]

qt_set_property -object $paletteCanvas -property paletteName -value plasma

#---

set paletteControl [qt_create_widget -type CQChartsPaletteControl -name paletteControl]

set paletteControlAnn [create_charts_widget_annotation -plot $plot -id paletteControl \
  -rectangle [list 51 1 99 49 V] -widget $paletteControl]

set paletteControl [get_charts_data -annotation $paletteControl -name widget_path]

#---

set interfaceControl [qt_create_widget -type CQChartsInterfaceControl -name interfaceControl]

set interfaceControlAnn [create_charts_widget_annotation -plot $plot -id interfaceControl \
  -rectangle [list 51 50 99 99 V] -widget $interfaceControl]

set interfaceControl [get_charts_data -annotation $interfaceControl -name widget_path]

#---

connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot
