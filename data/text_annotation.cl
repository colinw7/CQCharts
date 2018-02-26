load -csv arrowstyle.csv

modelId = _rc

plot -type xy -columns "x=0,y=1" -title "This plot has a very long title which we might want to format on multiple lines"

plotId = _rc

text_shape -x 0 -y -60 -text "One"
text_shape -x 0 -y -50 -text "Two"   -color red
text_shape -x 0 -y -40 -text "Three" -color green -alpha 0.5
text_shape -x 0 -y -30 -text "Four"  -color blue  -angle 45

text_shape -x 0 -y 0 -text "The rain in spain falls mainly on the plain. She sell sea shells on the sea shore. The quick brown fox jumps over the lazy dog." -border 1 -background 1 -corner_size 12

textAnnotationId = _rc
