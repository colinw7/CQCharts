load_model -csv data/arrowstyle.csv
modelId = _rc

create_plot -type xy -columns "x=0,y=1,vectorY=2" -bool "vectors=1"
plotId = _rc
