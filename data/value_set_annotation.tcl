set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set values [list 10 25 37 42 55 45 33 21 16]

set ann [create_charts_value_set_annotation -plot $plotId -rectangle {25 25 75 75} -values $values]
