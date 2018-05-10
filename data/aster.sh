CQChartsTest -ceil -exec aster.cl

CQChartsTest -csv data/aster_data.csv -first_line_header \
 -column_type "4#color" \
 -type pie -properties "aster=1,grid.visible=1,donut=1,innerRadius=0.2" \
 -columns "id=0,width=2,data=3,label=5,color=4"
