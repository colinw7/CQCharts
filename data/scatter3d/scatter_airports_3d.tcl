set model [load_charts_model -csv data/airports.csv -comment_header]

# iata,name,city,state,country,latitude,longitude
set plot [create_charts_plot -model $model -type scatter3d \
  -columns {{x longitude} {y latitude} {z @GROUP} {group state} {label name} {id iata} {tips {city state}}} \
  -title "airports"]
