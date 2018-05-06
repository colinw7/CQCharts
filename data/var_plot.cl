x = indarrI(100)
y = x/20.0
c = cos(y)
s = sin(y)

load -var x -var y -var c -var s
modelInd = _rc

add_plot -type xy -columns "x=0,y=2 3"
plotInd = _rc
