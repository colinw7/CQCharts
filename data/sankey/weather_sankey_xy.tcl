# set city ids
set cities      {seattle portland sanfrancisco losangeles sandiego}
set city_titles {Seattle Portland {San Francisco} {Los Angeles} {San Diego}}

# create id to name lookup arrays (TODO: use above)
set city_title(seattle) Seattle
set city_title(portland) Portland
set city_title(sanfrancisco) {San Francisco}
set city_title(losangeles) {Los Angeles}
set city_title(sandiego) {San Diego}

set months {January February March April May June July August September October November December}

#---

set conditions [list {Partially cloudy} {Rain, Overcast} {Overcast} {Rain, Partially cloudy} {Rain} {Snow, Overcast} {Snow, Partially cloudy} {Clear}]

set condition_color(0) {#E7D8C9}
set condition_color(1) {#98C1D9}
set condition_color(2) {#DDDDDD}
set condition_color(3) {#E7D8C9}
set condition_color(4) {#98C1D9}
set condition_color(5) {#C2DFE3}
set condition_color(6) {#C2DFE3}
set condition_color(7) {#FFF3B0}

proc conditionIndex { condition } {
  set ind 0

  foreach condition1 $::conditions {
    if {$condition eq $condition1} {
      return $ind
    }

    incr ind
  }

  return -1
}

proc conditionColor { condition } {
  set ind [conditionIndex $condition]

  if {$ind >= 0} {
    return $::condition_color($ind)
  }

  echo "$condition not found"

  return {#000000}
}

#---

set current_city       seattle
set current_city_title Seattle
set current_month      January
set current_month_num  1

#---

# create model for each city
foreach city $cities {
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
  set model($city) [load_charts_model -csv data/${city}_weather.csv -first_line_header]

  set_charts_data -model $model($city) -column 1 -name column_type \
    -value {{time} {format {%m/%d/%Y}}}

  # rename columns for shorter tip strings
  set_charts_data -model $model($city) -header -column 1 -name value -value "Date"
  set_charts_data -model $model($city) -header -column 2 -name value -value "Min Temp"
  set_charts_data -model $model($city) -header -column 3 -name value -value "Max Temp"
  set_charts_data -model $model($city) -header -column 4 -name value -value "Temp"
  set_charts_data -model $model($city) -header -column 6 -name value -value "Humidity"

  # create sankey and xy model for each month
  for {set month 1} {$month <= 12} {incr month} {
    set mmodel [create_charts_data_model -rows 31 -columns 5]

    set_charts_data -model $mmodel -name name -value "sankey_${city}_${month}"

    set sankey_month_model($city,$month) $mmodel

    set_charts_data -model $mmodel -header -column 0 -name value -value "Conditions"
    set_charts_data -model $mmodel -header -column 1 -name value -value "Day"
    set_charts_data -model $mmodel -header -column 2 -name value -value "Value"
    set_charts_data -model $mmodel -header -column 3 -name value -value "PathId"
    set_charts_data -model $mmodel -header -column 4 -name value -value "Attributes"

    # set named colors and named images for conditions
    set_charts_data -model $mmodel -column 0 -name column_type \
      -value {{string} {named_images {
               {{Partially cloudy} {svg:data/weather_cloudy.svg}}
               {{Rain, Overcast} {svg:data/weather_rain.svg}}
               {{Overcast} {svg:data/weather_overcast.svg}}
               {{Rain, Partially cloudy} {svg:data/weather_rain.svg}}
               {{Rain} {svg:data/weather_rain.svg}}
               {{Snow, Overcast} {svg:data/weather_snow.svg}}
               {{Snow, Partially cloudy} {svg:data/weather_snow.svg}}
               {{Clear} {svg:data/weather_clear.svg}}
              }}}

    #---

    set mmodel [create_charts_data_model -rows 31 -columns 3]

    set_charts_data -model $mmodel -name name -value "xy_${city}_${month}"

    set xy_month_model($city,$month) $mmodel

    set_charts_data -model $mmodel -header -column 0 -name value -value "Date"
    set_charts_data -model $mmodel -header -column 1 -name value -value "Min Temp"
    set_charts_data -model $mmodel -header -column 2 -name value -value "Max Temp"
  }

  set nr [get_charts_data -model $model($city) -name num_rows]

  for {set r 0} {$r < $nr} {incr r} {
    set month [get_charts_data -model $model($city) -row $r -column {1[%m]} -name value]

    if {[string index $month 0] == "0"} {
      set month [string range $month 1 end]
    }

    set day       [get_charts_data -model $model($city) -row $r -column {1[%d]} -name value]
    set min_temp  [get_charts_data -model $model($city) -row $r -column 2 -name value]
    set max_temp  [get_charts_data -model $model($city) -row $r -column 3 -name value]
    set condition [get_charts_data -model $model($city) -row $r -column 24 -name value]

    if {[string index $day 0] == "0"} {
      set day [string range $day 1 end]
    }

    set iday [expr {$day - 1}]

    #---

    set color  [conditionColor $condition]
   #set pathId [conditionIndex $condition]
    set pathId $day

    set_charts_data -model $sankey_month_model($city,$month) \
      -row $iday -column 0 -name value -value $condition
    set_charts_data -model $sankey_month_model($city,$month) \
      -row $iday -column 1 -name value -value $day
    set_charts_data -model $sankey_month_model($city,$month) \
      -row $iday -column 2 -name value -value 1
    set_charts_data -model $sankey_month_model($city,$month) \
      -row $iday -column 3 -name value -value $pathId
    set_charts_data -model $sankey_month_model($city,$month) \
      -row $iday -column 4 -name value -value "color=$color,src_color=$color,dest_color=$color"

    #---

    set_charts_data -model $xy_month_model($city,$month) \
      -row $iday -column 0 -name value -value $day
    set_charts_data -model $xy_month_model($city,$month) \
      -row $iday -column 1 -name value -value $min_temp
    set_charts_data -model $xy_month_model($city,$month) \
      -row $iday -column 2 -name value -value $max_temp
  }
}

#---

# create sankey plot
set sankey_plot [create_charts_plot -type sankey -model $sankey_month_model($current_city,1) \
  -columns {{from 1} {to 0} {value 2} {pathId 3} {attributes 4}} \
  -title "Weather for $current_city_title in $current_month"]

set_charts_property -plot $sankey_plot -name options.orientation -value Vertical
#set_charts_property -plot $sankey_plot -name options.valueLabel  -value 1
set_charts_property -plot $sankey_plot -name text.internal       -value 0

set_charts_property -plot $sankey_plot -name plotBox.clip         -value 0
set_charts_property -plot $sankey_plot -name plotBox.fill.visible -value 0
set_charts_property -plot $sankey_plot -name dataBox.clip         -value 0
set_charts_property -plot $sankey_plot -name dataBox.fill.visible -value 0

#set_charts_property -plot $sankey_plot -name margins.outer.left   -value 5%
#set_charts_property -plot $sankey_plot -name margins.outer.right  -value 5%
#set_charts_property -plot $sankey_plot -name margins.outer.top    -value 5%
#set_charts_property -plot $sankey_plot -name margins.outer.bottom -value 5%

#---

# create xy plot
set xy_plot [create_charts_plot -type xy -model $xy_month_model($current_city,1) \
  -columns {{x 0} {y {1 2}}} \
  -title "Weather for $current_city_title in $current_month"]

set view [get_charts_data -plot $xy_plot -name view]

set_charts_property -view $view -name probe.pos -value MIN

set_charts_property -plot $xy_plot -name xaxis.valueType      -value INTEGER
set_charts_property -plot $xy_plot -name xaxis.majorIncrement -value 1

#---

set_charts_property -plot $sankey_plot -name range.view -value {0 20 100 100}
set_charts_property -plot $xy_plot     -name range.view -value {0 0 100 20}

#---

proc updatePlotTitle { } {
  set_charts_property -plot $::sankey_plot -name title.text.string \
    -value "Weather for $::current_city_title in $::current_month"

  set_charts_property -plot $::xy_plot -name title.text.string \
    -value "Weather for $::current_city_title in $::current_month"
}

proc updatePlotModel { } {
  set_charts_data -plot $::sankey_plot -name model \
    -value $::sankey_month_model($::current_city,$::current_month_num)

  set_charts_data -plot $::xy_plot -name model \
    -value $::xy_month_model($::current_city,$::current_month_num)
}

proc cityComboChanged { city_title } {
  echo "cityComboChanged $city_title"

  set ::current_city_title $city_title

  set i 0

  foreach city $::cities city_title $::city_titles {
    if {$city_title == $::current_city_title} {
      set ::current_city $city
      break
    }
  }

  updatePlotTitle
  updatePlotModel
}

proc monthComboChanged { month_name } {
  echo "monthComboChanged $month_name"

  set ::current_month $month_name

  set month_num 1

  foreach month $::months {
    if {$month == $month_name} {
      set ::current_month_num $month_num
      break
    }

    incr month_num
  }

  updatePlotTitle
  updatePlotModel
}

#---

set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QHBoxLayout -parent $frame]

set cityCombo [qt_create_widget -type QComboBox -name cityCombo]

qt_set_property -object $cityCombo -property items -value $city_titles

qt_connect_widget -name $cityCombo -signal "currentIndexChanged(const QString &)" \
  -proc cityComboChanged

set monthCombo [qt_create_widget -type QComboBox -name monthCombo]

qt_set_property -object $monthCombo -property items -value $months

qt_connect_widget -name $monthCombo -signal "currentIndexChanged(const QString &)" \
  -proc monthComboChanged

qt_add_child_widget -parent $frame -child $cityCombo
qt_add_child_widget -parent $frame -child $monthCombo

set annotation [create_charts_widget_annotation -plot $sankey_plot -id frame \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $sankey_plot -name view_path]

set cityCombo  "$view_path|frame|$cityCombo"
set monthCombo "$view_path|frame|$monthCombo"
