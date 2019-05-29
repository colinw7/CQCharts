set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1} {vectorY 2}} \
 -properties {{points.visible 1} {vectors.visible 1}}]
