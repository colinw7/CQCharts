# load model
set model [load_charts_model -csv data/who_suicide_statistics.csv -first_line_header]

# rename some values
process_charts_model -model $model -replace "United States of America" -column 0 -value "United States"
process_charts_model -model $model -replace "Russian Federation" -column 0 -value "Russia"
process_charts_model -model $model -replace "Republic of Korea" -column 0 -value "North Korea"
process_charts_model -model $model -replace "Republic of Moldova" -column 0 -value "Moldova"
process_charts_model -model $model -replace "Venezuela (Bolivarian Republic of)" -column 0 -value "Venezuela"

# rename columns
set_charts_data -model $model -column sex -header -name value -value gender
set_charts_data -model $model -column suicides_no -header -name value -value suicides

# get null value percentage
set nc [get_charts_data -model $model -name num_columns]
set nr [get_charts_data -model $model -name num_rows]

echo "Null Value Percentage per Column"

for {set c 0} {$c < $nc} {incr c} {
  set column_name [get_charts_data -model $model -column $c -header -name value]

  set num_null [get_charts_data -model $model -column $c -name details.num_null]

  echo "  $column_name : [expr {(100.0*$num_null)/$nr}]"
}

# get correlation of suicides and population
set corrModel [create_charts_correlation_model -model $model -columns {suicides population}]

set corr [get_charts_data -model $corrModel -row 0 -column 1 -name value]

echo ""
echo "Correlation between Suicides and Population"
echo "  $corr"

# display suicides > 5000 in 15-24 age group
echo ""
echo "Suicides in 15-24 age group"

set manyYoungSuicidesModel [copy_charts_model -model $model -filter {$suicides > 5000 && $age == "15-24 years"}]

sort_charts_model -model $manyYoungSuicidesModel -column suicides -decreasing

write_charts_model -model $manyYoungSuicidesModel -max_rows 40 -columns {country suicides gender year}

# display top 10 countries wrt suicides
echo ""
echo "Top 10 Counties wrt Suicides"

set countrySuicidesModel \
 [flatten_charts_model -model $model -group country -column_ops {{sum suicides}}]

sort_charts_model -model $countrySuicidesModel -column {suicides (Sum)} -decreasing

write_charts_model -model $countrySuicidesModel -max_rows 10 -columns {country {suicides (Sum)}}

# display bottom 10 countries wrt suicides
echo ""
echo "Bottom 10 Counties wrt Suicides"

sort_charts_model -model $countrySuicidesModel -column {suicides (Sum)}

write_charts_model -model $countrySuicidesModel -max_rows 10 -columns {country {suicides (Sum)}}

# display highest suides by year
echo ""
echo "Highest suicides by year"

set yearSuicidesModel [flatten_charts_model -model $model -group year -column_ops {{sum suicides}}]

sort_charts_model -model $yearSuicidesModel -column {suicides (Sum)} -decreasing

write_charts_model -model $yearSuicidesModel -max_rows 10 -columns {year {suicides (Sum)}}

# display suicides by gender
echo ""
echo "Highest suicides by gender"

set genderSuicidesStatsModel [flatten_charts_model -model $model -group gender -column_ops {{min suicides} {median suicides} {max suicides} {sum suicides}}]

write_charts_model -model $genderSuicidesStatsModel -max_rows 10

# replace null values
process_charts_model -model $model -replace_null -column suicides -value 0
process_charts_model -model $model -replace_null -column population -value 1664090

#---

if {0} {
set view1 [create_charts_view]

# display scatter plot of population vs suicides
set populationSuicidesScatterPlot [create_charts_plot -view $view1 -model $model -type scatter -columns {{x population} {y suicides}}]

# display scatter plot of year vs suicides
set yearSuicidesScatterPlot [create_charts_plot -view $view1 -model $model -type scatter -columns {{x year} {y suicides}}]

place_charts_plots -view $view1 -columns 2 [list $populationSuicidesScatterPlot $yearSuicidesScatterPlot]
}

#---

if {0} {
set view2 [create_charts_view]

# display box plot of suicides per country
set suicidesCountryBoxPlot [create_charts_plot -view $view2 -model $model -type boxplot -columns {{values suicides} {group country}}]

set_charts_property -plot $suicidesCountryBoxPlot -name filter.expression -value {$suicides > 1000}
set_charts_property -plot $suicidesCountryBoxPlot -name options.orientation -value horizontal
set_charts_property -plot $suicidesCountryBoxPlot -name labels.visible -value 0
set_charts_property -plot $suicidesCountryBoxPlot -name key.visible -value 0

# display box plot of suicides per gender
set suicidesGenderBoxPlot [create_charts_plot -view $view2 -model $model -type boxplot -columns {{values suicides} {group gender}}]

set_charts_property -plot $suicidesGenderBoxPlot -name options.orientation -value horizontal
set_charts_property -plot $suicidesGenderBoxPlot -name labels.visible -value 0
set_charts_property -plot $suicidesGenderBoxPlot -name key.visible -value 0

# display box plot of suicides per age
set suicidesAgeBoxPlot [create_charts_plot -view $view2 -model $model -type boxplot -columns {{values suicides} {group age}}]

set_charts_property -plot $suicidesAgeBoxPlot -name options.orientation -value horizontal
set_charts_property -plot $suicidesAgeBoxPlot -name labels.visible -value 0
set_charts_property -plot $suicidesAgeBoxPlot -name key.visible -value 0

place_charts_plots -view $view2 -columns 3 [list $suicidesCountryBoxPlot $suicidesGenderBoxPlot $suicidesAgeBoxPlot]
}

#---

if {0} {
# display suicides per country
set view3 [create_charts_view]

set worldModel [load_charts_model -csv data/world.csv -comment_header \
  -column_type {{{1 polygon_list}}}]

set_charts_data -model $worldModel -column 0 -header -name value -value country

set worldSuicidesModel [join_charts_model -models [list $worldModel $countrySuicidesModel] -columns {country}]

set worldSuicidesGeometryPlot [create_charts_plot -view $view3 -model $worldSuicidesModel -type geometry -columns {{geometry geometry} {value 2} {name country}}]
}

#---

if {0} {
# display barchart of top 10 coutries wrt suicides
sort_charts_model -model $countrySuicidesModel -column {suicides (Sum)} -decreasing

set countrySuicidesTop10Model [copy_charts_model -model $countrySuicidesModel -rows 10]

set view4 [create_charts_view]

set yeatSuicidesSumBarChart [create_charts_plot -view $view4 -model $countrySuicidesTop10Model -type barchart -columns {{name country} {value 1}}]
}

#---

if {0} {
# display xy plot of top 10 coutries wrt suicides
set view5 [create_charts_view]

sort_charts_model -model $yearSuicidesModel -column {year}

set yeatSuicidesSumXYPlot [create_charts_plot -view $view5 -model $yearSuicidesModel -type xy -columns {{x year} {y {{suicides (Sum)}}}}]
}

#---

# display piechart of suicides wrt gender
if {0} {
set view6 [create_charts_view]

if {0} {
set suicidesGenderPieChart [create_charts_plot -view $view6 -model $genderSuicidesStatsModel -type piechart -columns {{values {{suicides (Sum)}}} {label gender}}]
}

if {0} {
set suicidesGenderBarChart [create_charts_plot -view $view6 -model $genderSuicidesStatsModel -type barchart -columns {{values {{suicides (Sum)}}} {name gender}}]
}
}

#---

if {0} {
set ageCountModel \
 [flatten_charts_model -model $model -group age -column_ops {{count age}}]

write_charts_model -model $ageCountModel -max_rows 10
}

#---

set usaModel    [copy_charts_model -model $model -filter {$country == "United States"}]
set russiaModel [copy_charts_model -model $model -filter {$country == "Russia"}]
set japanModel  [copy_charts_model -model $model -filter {$country == "Japan"}]

if {0} {
set usaYearModel [flatten_charts_model -model $usaModel -group year -column_ops {{sum suicides}}]
set russiaYearModel [flatten_charts_model -model $russiaModel -group year -column_ops {{sum suicides}}]
set japanYearModel [flatten_charts_model -model $japanModel -group year -column_ops {{sum suicides}}]

set view7 [create_charts_view]

set plot10 [create_charts_plot -view $view7 -model $usaYearModel -type barchart -columns {{values {{suicides (Sum)}}} {name year}}]
set plot11 [create_charts_plot -view $view7 -model $russiaYearModel -type barchart -columns {{values {{suicides (Sum)}}} {name year}}]
set plot12 [create_charts_plot -view $view7 -model $japanYearModel -type barchart -columns {{values {{suicides (Sum)}}} {name year}}]

place_charts_plots -view $view7 -rows 3 [list $plot10 $plot11 $plot12]
}

#---

if {0} {
set usaAgeModel [flatten_charts_model -model $usaModel -group age -column_ops {{sum suicides}}]
set russiaAgeModel [flatten_charts_model -model $russiaModel -group age -column_ops {{sum suicides}}]
set japanAgeModel [flatten_charts_model -model $japanModel -group age -column_ops {{sum suicides}}]

set view8 [create_charts_view]

set plot13 [create_charts_plot -view $view8 -model $usaAgeModel -type barchart -columns {{values {{suicides (Sum)}}} {name age}}]
set plot14 [create_charts_plot -view $view8 -model $russiaAgeModel -type barchart -columns {{values {{suicides (Sum)}}} {name age}}]
set plot15 [create_charts_plot -view $view8 -model $japanAgeModel -type barchart -columns {{values {{suicides (Sum)}}} {name age}}]

place_charts_plots -view $view8 -rows 3 [list $plot13 $plot14 $plot15]
}

#---

set view9 [create_charts_view]

#set plot16 [create_charts_plot -view $view9 -model $model -type scatter -columns {{x year} {y suicides}}]
#set plot17 [create_charts_plot -view $view9 -model $model -type scatter -columns {{x gender} {y suicides}}]

set plot17 [create_charts_plot -view $view9 -model $model -type boxplot \
  -columns {{group gender} {values suicides}}]

#---

if {0} {
set model7 [flatten_charts_model -model $model -group 1 -column_ops {{sum 4}}]
set model8 [flatten_charts_model -model $model -group 2 -column_ops {{sum 4}}]
}

#---

show_charts_manage_models_dlg
