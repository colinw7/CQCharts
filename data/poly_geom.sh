CQChartsTest \
 -csv data/polys.csv -comment_header \
 -type geometry -columns "name=0,geometry=1,value=2,color=3" \
 -column_type "1#polygon;3#color" \
 -plot_title "polygons" \
 -close_app
