set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

set arrowId [create_charts_arrow_annotation -plot $plotId -start {-200 -40} -end {500 40} \
 -fhead stealth -thead line -length 80px -line_width 20px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]
