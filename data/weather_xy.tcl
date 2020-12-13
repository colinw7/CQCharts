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
set condition_color(1) {#98c1d9}
set condition_color(2) {#DDDDDD}
set condition_color(3) {#E7D8C9}
set condition_color(4) {#98c1d9}
set condition_color(5) {#FFFFFF}
set condition_color(6) {#FFFFFF}
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

  # create model for each month
  for {set month 1} {$month <= 12} {incr month} {
    set month_model($city,$month) [create_charts_data_model -rows 31 -columns 3]

    set_charts_data -model $month_model($city,$month) -header -column 0 -name value -value "Date"
    set_charts_data -model $month_model($city,$month) -header -column 1 -name value -value "Min Temp"
    set_charts_data -model $month_model($city,$month) -header -column 2 -name value -value "Max Temp"
  }

  set nr [get_charts_data -model $model($city) -name num_rows]

  for {set r 0} {$r < $nr} {incr r} {
    set month [get_charts_data -model $model($city) -row $r -column {1[%m]} -name value]

    if {[string index $month 0] == "0"} {
      set month [string range $month 1 end]
    }

    set day      [get_charts_data -model $model($city) -row $r -column {1[%d]} -name value]
    set min_temp [get_charts_data -model $model($city) -row $r -column 2 -name value]
    set max_temp [get_charts_data -model $model($city) -row $r -column 3 -name value]

    if {[string index $day 0] == "0"} {
      set day [string range $day 1 end]
    }

    set iday [expr {$day - 1}]

    set_charts_data -model $month_model($city,$month) \
      -row $iday -column 0 -name value -value $day
    set_charts_data -model $month_model($city,$month) \
      -row $iday -column 1 -name value -value $min_temp
    set_charts_data -model $month_model($city,$month) \
      -row $iday -column 2 -name value -value $max_temp
  }
}

# create xy plot
set plot [create_charts_plot -type xy -model $month_model($current_city,1) \
  -columns {{x 0} {y {1 2}}} \
  -title "Weather for $current_city_title in $current_month"]

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name probe.pos -value MIN

set_charts_property -plot $plot -name xaxis.valueType      -value INTEGER
set_charts_property -plot $plot -name xaxis.majorIncrement -value 1

#---

proc updatePlotTitle { } {
  set_charts_property -plot $::plot -name title.text.string \
    -value "Weather for $::current_city_title in $::current_month"
}

proc updatePlotModel { } {
  set_charts_data -plot $::plot -name model \
    -value $::month_model($::current_city,$::current_month_num)
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

set annotation [create_charts_widget_annotation -plot $plot -id frame \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $annotation -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set cityCombo  "$view_path|frame|$cityCombo"
set monthCombo "$view_path|frame|$monthCombo"
