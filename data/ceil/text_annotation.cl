proc annotationSlot(viewId, plotId, id)
  print "annotationSlot", viewId, plotId, id

  get_property -annotation $id -name position
  print _rc

  get_property -annotation $id -name text
  print _rc
endproc

load_model -csv data/arrowstyle.csv
modelId = _rc

create_plot -type xy -columns "x=0,y=1" -title "This plot has a very long title which we might want to format on multiple lines"
plotId = _rc

create_text_shape -plot $plotId -id one -x 0 -y -60 -text "One"
textAnnotationId1 = _rc

create_text_shape -plot $plotId -id two -x 0 -y -50 -text "Two"   -color red
textAnnotationId2 = _rc

create_text_shape -plot $plotId -id three -x 0 -y -40 -text "Three" -color green -alpha 0.5
textAnnotationId3 = _rc

create_text_shape -plot $plotId -id four -x 0 -y -30 -text "Four"  -color blue  -angle 45
textAnnotationId4 = _rc

create_text_shape -plot $plotId -id long -x 0 -y 0 -text "The rain in spain falls mainly on the plain. She sell sea shells on the sea shore. The quick brown fox jumps over the lazy dog." -border 1 -background 1 -corner_size 12
textAnnotationId5 = _rc

connect_chart -view $viewId -plot $plotId1 -from annotationPressed -to annotationSlot
