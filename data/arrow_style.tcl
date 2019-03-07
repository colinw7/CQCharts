set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns "x=0,y=1,vectorY=2" -parameter "points=1" -parameter "vectors=1"]
