CQChartsTest \
 -csv data/cities.csv -first_line_header \
 -type scatter \
 -columns "name=0,x=1,y=2,symbolSize=3,color=4" \
 -column_type "3#real:min=0,max=1;4#color" -plot_title "Cities" \
 -close_app
