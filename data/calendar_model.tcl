# year,month,day
set model [load_charts_model -csv data/calendar.csv -first_line_header]
#write_charts_model -model $model

set months [list "January" "February" "March" "April" "May" "June" "July" "August" "September" \
                 "October" "November" "December"]

set col1 [process_charts_model -model $model \
            -vars [list [list months $months]] \
            -add -expr {index($months,column(1)-1)} \
            -header Month]

echo "Add Month"
#write_charts_model -model $model

echo "Add Column (1)"
set col2 [process_charts_model -model $model -add -expr {1}]
#write_charts_model -model $model

echo "Add Column (2)"
set col3 [process_charts_model -model $model -add -expr {2}]
#write_charts_model -model $model

echo "Delete Column (2)"
process_charts_model -model $model -delete -column $col3
#write_charts_model -model $model

echo "Modify Column (1 to 3)"
process_charts_model -model $model -modify -column $col2 -expr {3}
#write_charts_model -model $model

echo "Replace Column (3 to 4)"
process_charts_model -model $model -replace {3} -column $col2 -value {4}
#write_charts_model -model $model

echo [get_charts_data -model $model -column $col2 -name null_value]
set_charts_data -model $model -column $col2 -name null_value -value "<null>"
echo [get_charts_data -model $model -column $col2 -name null_value]

echo "Replace Column (4 to null)"
process_charts_model -model $model -replace {4} -column $col2 -value {<null>}
#write_charts_model -model $model

echo "Replace Column (null to 5)"
process_charts_model -model $model -replace_null -column $col2 -value {5}
#write_charts_model -model $model

#show_charts_manage_models_dlg

set view1 [create_charts_view]
