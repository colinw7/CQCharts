set modelId [load_charts_model -csv data/arrowstyle.csv]

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set arc [create_charts_arc_annotation -plot $plot \
 -start {{0 0} {1 1}} -end {{99 99} {100 100}}]
