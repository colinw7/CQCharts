set env(CQ_CHARTS_PEDANTIC) 0

set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

set viewId [get_charts_property -plot $plotId -name viewId]

set rectId1 [create_charts_rectangle_annotation -plot $plotId -id rectId1 -tip "Rectangle 1" \
  -start {0 0} -end {0 0} -filled 1 -fill_color red -fill_alpha 0.5]

set rectId2 [create_charts_rectangle_annotation -plot $plotId -id rectId2 -tip "Rectangle 1" \
  -rectangle {0 0 0 0} -filled 1 -fill_color red -fill_alpha 0.5]

set arrowId1 [create_charts_arrow_annotation -plot $plotId -start {0 0} -end {0 0}]

set ellipseId1 [create_charts_ellipse_annotation -plot $plotId -id ellipseId1 \
  -center {0 0} -rx -5 -ry -6]
