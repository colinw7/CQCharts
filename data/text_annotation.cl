load -csv arrowstyle.csv

modelId = _rc

plot -type xy -columns "x=0,y=1" -title "Annotations"

plotId = _rc

text_shape -x 0 -y -60 -text "One"
text_shape -x 0 -y -50 -text "Two"   -color red
text_shape -x 0 -y -40 -text "Three" -color green -alpha 0.5
text_shape -x 0 -y -30 -text "Four"  -color blue  -angle 45

textAnnotationId = _rc
