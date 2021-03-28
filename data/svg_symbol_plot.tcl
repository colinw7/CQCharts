create_charts_symbol_set -name svg

add_charts_symbol -set svg -svg data/path.svg -name path
add_charts_symbol -set svg -svg data/weather_clear.svg -name path
add_charts_symbol -set svg -svg data/weather_cloudy.svg -name weather_cloudy
add_charts_symbol -set svg -svg data/weather_overcast.svg -name weather_overcast
add_charts_symbol -set svg -svg data/weather_rain.svg -name weather_rain
add_charts_symbol -set svg -svg data/weather_snow.svg -name weather_snow

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x petalLength} {y sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}} \
  -title "Scatter Group"]
