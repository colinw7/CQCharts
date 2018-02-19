load -csv arrowstyle.csv

modelId = _rc

plot -type xy -columns "x=0,y=1"

plotId = _rc

annotation -type text -x 0 -y 0 -text "Test Annotation"

testAnnotationId = _rc

annotation -type arrow -x1 0 -y1 0 -x2 100 -y2 10

arrowAnnotationId = _rc
