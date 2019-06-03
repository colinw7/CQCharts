# load model

set model [load_charts_model -csv data/bikes.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {{time} {format %d/%m/%Y}}

#---

# add day number to model (start)
set day_col [process_charts_model -model $model -add -expr {timeval(0,"%u")} -header Day]

#---

set model1 [create_charts_folded_model -model $model -column Day]

echo "Folded Model"

write_charts_model -model $model1

#---

set model2 [create_charts_collapse_model -model $model1 -sum "3 4 5 6 7 8 9 10"]

echo "Collapse Model"

write_charts_model -model $model2

#---

set model3 [create_charts_stats_model -model $model -columns "2 3 4 5 6 7 8 9"]

echo "Stats Model"

write_charts_model -model $model3

#---

#set view1 [create_charts_view]

#set plot1 [create_charts_plot -view $view1 -type barchart -model $model2 \
# -columns {{value {3 4 5 6 7 8 9 10}} {name 0}}]

set view2 [create_charts_view]

set plot2 [create_charts_plot -view $view2 -type distribution -model $model2 \
 -columns {{value {3 4 5 6 7 8 9 10}} {name 0} {color 0}}]

set_charts_property -plot $plot2 -name bucket.enabled    -value false
set_charts_property -plot $plot2 -name options.valueType -value SUM
#set_charts_property -plot $plot2 -name mapping.color.enabled -value 1

#set view3 [create_charts_view]

#set plot3 [create_charts_plot -view $view3 -type boxplot -model $model3 \
#  -columns {{x name} {min min} {lowerMedian lower_median} {median median} \
#            {upperMedian upper_median} {max max} {outliers outliers}}]
