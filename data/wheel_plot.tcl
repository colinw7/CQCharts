#  0 Address
#  1 Date time
#  2 Minimum Temperature
#  3 Maximum Temperature
#  4 Temperature
#  5 Dew Point
#  6 Relative Humidity
#  7 Heat Index
#  8 Wind Speed
#  9 Wind Gust
# 10 Wind Direction
# 11 Wind Chill
# 12 Precipitation
# 13 Precipitation Cover
# 14 Snow Depth
# 15 Visibility
# 16 Cloud Cover
# 17 Sea Level Pressure
# 18 Weather Type
# 19 Latitude
# 20 Longitude
# 21 Resolved Address
# 22 Name
# 23 Info
# 24 Conditions

set palette [create_charts_palette -palette red_green_blue]

set_charts_palette -palette red_green_blue -name defined_colors \
  -value {{0 #242be6} {0.25 #45bedc} {0.5 #a4e6a4} {0.75 #dfc243} {1 #e72b2b}}

#---

set cities {seattle portland sanfrancisco losangeles sandiego}

set city_title(seattle) Seattle
set city_title(portland) Portland
set city_title(sanfrancisco) {San Francisco}
set city_title(losangeles) {Los Angeles}
set city_title(sandiego) {San Diego}

set city_titles {Seattle Portland {San Francisco} {Los Angeles} {San Diego}}

foreach city $cities {
  set model($city) [load_charts_model -csv data/${city}_weather.csv -first_line_header]

  set_charts_data -model $model($city) -column 1 -name column_type \
    -value {{time} {format {%m/%d/%Y}}}

  set_charts_data -model $model($city) -column 24 -name column_type \
    -value {{string} {named_colors {
             {{Partially cloudy} {#DDDDDD}}
             {{Rain, Overcast} {#8080DD}}
             {{Overcast} {#CCCCCC}}
             {{Rain, Partially cloudy} {#8080DD}}
             {{Rain} {#8080DD}}
             {{Snow, Overcast} {#FFFFFF}}
             {{Snow, Partially cloudy} {#FFFFFF}}
             {{Clear} {#DDDD80}}
            }} {named_images {
             {{Partially cloudy} {svg:data/weather_cloudy.svg}}
             {{Rain, Overcast} {svg:data/weather_rain.svg}}
             {{Overcast} {svg:data/weather_overcast.svg}}
             {{Rain, Partially cloudy} {svg:data/weather_rain.svg}}
             {{Rain} {svg:data/weather_rain.svg}}
             {{Snow, Overcast} {svg:data/weather_snow.svg}}
             {{Snow, Partially cloudy} {svg:data/weather_snow.svg}}
             {{Clear} {svg:data/weather_clear.svg}}
            }}}

  set_charts_data -model $model($city) -header -column 1 -name value -value "Date"
  set_charts_data -model $model($city) -header -column 2 -name value -value "Min Temp"
  set_charts_data -model $model($city) -header -column 3 -name value -value "Max Temp"
  set_charts_data -model $model($city) -header -column 4 -name value -value "Temp"
  set_charts_data -model $model($city) -header -column 6 -name value -value "Humidity"
}

set plot [create_charts_plot -type wheel -model $model(seattle) \
  -columns {{x 1} {y 4} {min 2} {max 3} {innerBar 24} {outerBar 6} {outerBubble 12}} \
  -title "Weather for Seattle"]

set_charts_property -plot $plot -name options.tempPalette -value red_green_blue

set_charts_property -plot $plot -name plotBox.clip         -value 0
set_charts_property -plot $plot -name plotBox.fill.visible -value 0
set_charts_property -plot $plot -name dataBox.clip         -value 0
set_charts_property -plot $plot -name dataBox.fill.visible -value 0

set_charts_property -plot $plot -name margins.outer.left   -value 2.5%
set_charts_property -plot $plot -name margins.outer.right  -value 2.5%
set_charts_property -plot $plot -name margins.outer.top    -value 5%
set_charts_property -plot $plot -name margins.outer.bottom -value 0

#---

proc cityComboChanged { title } {
  #echo "cityComboChanged $title"
  set_charts_property -plot $::plot -name title.text.string -value "Weather for $title"

  set i 0

  foreach city $::cities city_title $::city_titles {
    if {$title == $city_title} {
      #echo "$city $city_title"
      set_charts_data -plot $::plot -name model -value $::model($city)
      break
    }
  }
}

set cityCombo [qt_create_widget -type QComboBox -name cityCombo]

qt_set_property -object $cityCombo -property items -value $city_titles

qt_connect_widget -name $cityCombo -signal "currentIndexChanged(const QString &)" \
  -proc cityComboChanged

set annotation [create_charts_widget_annotation -plot $plot -id cityCombo \
  -position [list 100 100 V] -widget $cityCombo]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}
