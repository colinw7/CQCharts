set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

set arrowId1 [create_charts_arrow_annotation -plot $plotId -start {-200  50} -end {200  50} \
 -fhead 0 -thead 1 -filled 1 -angle 30 -back_angle 45  -length 4V -stroke_width 3px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId2 [create_charts_arrow_annotation -plot $plotId -start {-200  40} -end {200  40} \
 -fhead 0 -thead 1 -filled 0 -angle 15                 -length 4V -stroke_width 3px \
 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId3 [create_charts_arrow_annotation -plot $plotId -start {-200  30} -end {200  30} \
 -fhead 0 -thead 1 -filled 1 -angle 15 -back_angle 45  -length 4V -stroke_width 3px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId4 [create_charts_arrow_annotation -plot $plotId -start {-200  20} -end {200  20} \
 -fhead 0 -thead 1 -filled 1 -angle 15                 -length 4V -stroke_width 3px \
 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId5 [create_charts_arrow_annotation -plot $plotId -start {-200  10} -end {200  10} \
 -fhead 1 -thead 1 -filled 1 -angle 15 -back_angle 135 -length 4V -stroke_width 3px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId6 [create_charts_arrow_annotation -plot $plotId -start {-200   0} -end {200   0} \
 -fhead 0 -thead 1 -angle 15 -back_angle 135 -length 4V -stroke_width 3px \
 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId7 [create_charts_arrow_annotation -plot $plotId -start {-200 -10} -end {200 -10} \
 -fhead 0 -thead 0                                     -length 4V -stroke_width 3px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId8 [create_charts_arrow_annotation -plot $plotId -start {-200 -20} -end {200 -20} \
 -fhead 1 -thead 1 -filled 0 -angle 90                 -length 4V -stroke_width 3px \
 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId9 [create_charts_arrow_annotation -plot $plotId -start {-200 -30} -end {200 -30} \
 -fhead triangle -thead stealth -length {4V 2V} -stroke_width 8px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId10 [create_charts_arrow_annotation -plot $plotId -start {-200 -40} -end {200 -40} \
 -fhead diamond -thead line -length 4V -stroke_width 11px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]
