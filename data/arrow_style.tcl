set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1,vectorY=2" -parameter "vectors=1"]
