set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set values [list [list 0 0] [list 10 10] [list 20 20] [list 30 20] [list 40 10] [list 50 0]]

set ann [create_charts_point_set_annotation -plot $plotId -values $values]
