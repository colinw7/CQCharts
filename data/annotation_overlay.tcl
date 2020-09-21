set model [load_charts_model -csv data/multi_name_value.csv -first_line_header]
  
set plot1 [create_charts_plot -type scatter -columns {{x 0} {y 1}} -title "Scatter Plot 1"]
set plot2 [create_charts_plot -type scatter -columns {{x 2} {y 3}} -title "Scatter Plot 2"]

set rect1 [create_charts_rectangle_annotation -plot $plot1 -id rect1 -tip "Rectangle 1" \
  -start {1 1} -end {2 1.5} -filled 1 -fill_color red -fill_alpha 0.5]
set rect2 [create_charts_rectangle_annotation -plot $plot2 -id rect2 -tip "Rectangle 2" \
  -start {2 2} -end {2.5 3} -filled 1 -fill_color green -fill_alpha 0.5]

group_charts_plots -overlay [list $plot1 $plot2]
