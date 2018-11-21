# Scatter Plot

CQChartsTest \
 -csv data/cities.csv -first_line_header \
 -type scatter -columns "name=0,x=1,y=2,symbolSize=3,color=4" \
 -column_type "4#color" \
 -plot_title "Cities" \
 -properties "symbol.map.enabled=1"

CQChartsTest \
 -tsv data/scatter.tsv -first_line_header \
 -type scatter -columns "name=4,x=0,y=1,symbolSize=2px" \
 -plot_title "scatter" \
 -properties "symbol.map.enabled=1,symbol.size=12,symbol.stroke.alpha=0.3,symbol.fill.alpha=0.5" \
 -view_properties "selectedHighlight.fill.enabled=1"

CQChartsTest \
 -csv data/bubble.csv -comment_header \
 -type scatter -columns "name=0,x=1,y=2,color=3,symbolSize=4px" \
 -plot_title "Scatter Plot" \
 -properties "symbol.map.enabled=1,color.map.enabled=1"

# TODO: ignore degree symbol in cities.dat ?
CQChartsTest \
 -tsv data/cities1.dat -comment_header -process "+column(2)/20000.0" \
 -type scatter -columns "x=4,y=3,name=0,fontSize=5" \
 -bool "textLabels=1,key=0" \
 -properties "font.map.enabled=1,dataLabel.position=CENTER"

CQChartsTest \
 -csv data/airports.csv -comment_header \
 -type scatter -columns "x=6,y=5,name=1" \
 -plot_title "airports"
