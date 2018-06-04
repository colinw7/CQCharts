set modelId [load_model -csv data/arrowstyle.csv]

set plotId [create_plot -type xy -columns "x=0,y=1" -title "Annotations"]

set rectId1 [create_rect_shape -plot $plotId -x1 0 -y1 -60 -x2 100 -y2 -40 \
  -background 1 -background_color red -background_alpha 0.5]

set rectId2 [create_rect_shape -plot $plotId -x1 0 -y1 -30 -x2 100 -y2 -10 \
  -border_color green -border_alpha 0.5 -border_width 3 -corner_size 8]

set rectId3 [create_rect_shape -plot $plotId -x1 0 -y1   0 -x2 100 -y2  20 \
  -border_color blue -border_alpha 0.5 -border_width 3 -border_sides tl]
