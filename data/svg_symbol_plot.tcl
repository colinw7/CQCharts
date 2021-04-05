create_charts_symbol_set -name svg

add_charts_symbol -set svg -svg data/path.svg -name path -filled 1 -styled 1
add_charts_symbol -set svg -svg data/weather_clear.svg -name weather_clear -filled 1 -styled 1
add_charts_symbol -set svg -svg data/weather_cloudy.svg -name weather_cloudy -filled 1 -styled 1
add_charts_symbol -set svg -svg data/weather_overcast.svg -name weather_overcast -filled 1 -styled 1
add_charts_symbol -set svg -svg data/weather_rain.svg -name weather_rain -filled 1 -styled 1
add_charts_symbol -set svg -svg data/weather_snow.svg -name weather_snow -filled 1 -styled 1

create_charts_symbol_set -name path

add_charts_symbol -set path -path "M 0 0 L 10 0 L 10 10" -filled 1 -name path1
add_charts_symbol -set path -path "M 0 0 L 0 10 L 10 10" -filled 1 -name path2

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x petalLength} {y sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}} \
  -title "Scatter Group"]
