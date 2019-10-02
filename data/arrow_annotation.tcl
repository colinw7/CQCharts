set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Annotations"]

set arrowId1 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 50} -end {200 50} \
 -fhead none -thead triangle \
 -angle {30 -1} -back_angle {45 -1} \
 -length {20px 30px} \
 -stroke_width 3px \
 -filled 1 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId2 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 40} -end {200 40} \
 -fhead triangle -thead none \
 -angle {-1 30} \
 -length {30px 45px} \
 -stroke_width 3px \
 -filled 0 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId3 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 30} -end {200 30} \
 -fhead none -thead none \
 -length {30px 45px} \
 -stroke_width 3px \
 -filled 1 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId4 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 20} -end {200 20} \
 -fhead triangle -thead triangle \
 -length {30px 45px} \
 -stroke_width 3px \
 -filled 1 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId5 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 10} -end {200 10} \
 -fhead stealth -thead stealth \
 -length {30px 45px} \
 -stroke_width 3px \
 -filled 1 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId6 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 0} -end {200 0} \
 -fhead diamond -thead diamond \
 -length {30px 45px} \
 -stroke_width 3px \
 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId7 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 -10} -end {200 -10} \
 -fhead line -thead line \
 -length {20px 30px} \
 -stroke_width 3px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId8 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 -20} -end {200 -20} \
 -fhead triangle -thead line \
 -length {20px 30px} \
 -stroke_width 3px \
 -filled 0 -stroke_color {palette 0.6} -fill_color {palette 0.6}]

set arrowId9 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 -30} -end {200 -30} \
 -fhead line -thead triangle \
 -length {20px 30px} \
 -stroke_width 8px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId10 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 -40} -end {200 -40} \
 -fhead none -thead line \
 -length {20px 30px} \
 -stroke_width 11px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]

set arrowId11 [create_charts_arrow_annotation -plot $plotId \
 -start {-200 -50} -end {200 -50} \
 -fhead 1 -thead 1 \
 -angle {30 45} -back_angle {40 60} \
 -length {40px 30px} \
 -stroke_width 5px \
 -stroke_color {palette 0.3} -fill_color {palette 0.3}]
