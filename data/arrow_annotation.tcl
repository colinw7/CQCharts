set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "Annotations"]

set arrowId1 [create_arrow_shape -plot $plotId -x1 -200 -y1   0 -x2 200 -y2   0 \
 -fhead 0 -thead 1 -filled 1 -angle 30 -back_angle 45  -length 4V -line_width 3px \
 -stroke_color palette:0.3 -fill_color palette:0.3]

set arrowId2 [create_arrow_shape -plot $plotId -x1 -200 -y1 -10 -x2 200 -y2 -10 \
 -fhead 0 -thead 1 -filled 0 -angle 15                 -length 4V -line_width 3px \
 -stroke_color palette:0.6 -fill_color palette:0.6]

set arrowId3 [create_arrow_shape -plot $plotId -x1 -200 -y1 -20 -x2 200 -y2 -20 \
 -fhead 0 -thead 1 -filled 1 -angle 15 -back_angle 45  -length 4V -line_width 3px \
 -stroke_color palette:0.3 -fill_color palette:0.3]

set arrowId4 [create_arrow_shape -plot $plotId -x1 -200 -y1 -30 -x2 200 -y2 -30 \
 -fhead 0 -thead 1 -filled 1 -angle 15                 -length 4V -line_width 3px \
 -stroke_color palette:0.6 -fill_color palette:0.6]

set arrowId5 [create_arrow_shape -plot $plotId -x1 -200 -y1 -40 -x2 200 -y2 -40 \
 -fhead 1 -thead 1 -filled 1 -angle 15 -back_angle 135 -length 4V -line_width 3px \
 -stroke_color palette:0.3 -fill_color palette:0.3]

set arrowId6 [create_arrow_shape -plot $plotId -x1 -200 -y1 -50 -x2 200 -y2 -50 \
 -fhead 0 -thead 1 -empty  1 -angle 15 -back_angle 135 -length 4V -line_width 3px \
 -stroke_color palette:0.6 -fill_color palette:0.6]

set arrowId7 [create_arrow_shape -plot $plotId -x1 -200 -y1 -60 -x2 200 -y2 -60 \
 -fhead 0 -thead 0                                     -length 4V -line_width 3px \
 -stroke_color palette:0.3 -fill_color palette:0.3]

set arrowId8 [create_arrow_shape -plot $plotId -x1 -200 -y1 -70 -x2 200 -y2 -70 \
 -fhead 1 -thead 1 -filled 0 -angle 90                 -length 4V -line_width 3px \
 -stroke_color palette:0.6 -fill_color palette:0.6]
