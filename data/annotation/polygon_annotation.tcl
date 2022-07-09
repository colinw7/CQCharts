set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 30 -ymax 30]

# Arkansas
create_charts_polygon_annotation -plot $plotId -id poly -points "0 0 10 10 20 10 30 20 20 20 10 20 0 10" -filled 1 -fill_color red -fill_alpha 0.5
