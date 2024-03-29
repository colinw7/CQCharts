set model1 [load_charts_model -data data/1.dat -comment_header]
set model2 [load_charts_model -data data/2.dat -comment_header]
set model3 [load_charts_model -data data/3.dat -comment_header]

set_charts_data -model $model1 -column 0 -header -name value -value "X1"
set_charts_data -model $model1 -column 1 -header -name value -value "Y1"

set_charts_data -model $model2 -column 0 -header -name value -value "X2"
set_charts_data -model $model2 -column 1 -header -name value -value "Y2"

set_charts_data -model $model3 -column 0 -header -name value -value "X3"
set_charts_data -model $model3 -column 1 -header -name value -value "Y3"

set plot1 [create_charts_plot -model $model1 -type scatter -columns {{x 0} {y 1}}]
set plot2 [create_charts_plot -model $model2 -type scatter -columns {{x 0} {y 1}}]
set plot3 [create_charts_plot -model $model3 -type scatter -columns {{x 0} {y 1}}]

set_charts_property -plot $plot1 -name state.selectable -value 0
set_charts_property -plot $plot2 -name state.selectable -value 0
set_charts_property -plot $plot3 -name state.selectable -value 0

group_charts_plots -overlay [list $plot1 $plot2 $plot3]

set rect1 [create_charts_rectangle_annotation -plot $plot1 -id rect1 \
  -start {-20 -8} -end {-10 -4}]
set rect2 [create_charts_rectangle_annotation -plot $plot2 -id rect1 \
  -start {-5 -2} -end {10 2}]
set rect3 [create_charts_rectangle_annotation -plot $plot3 -id rect1 \
  -start {10 4} -end {20 8}]

