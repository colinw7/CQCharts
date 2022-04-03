set df {{"AAA": [4, 5, 6, 7], "BBB": [10, 20, 30, 40], "CCC": [100, 50, -30, -50]}}

set model [load_charts_model -json @df]
write_charts_model -model $model -separator "\\t" -index 1

#---

#echo [get_charts_data -model $model -name data_model]
#echo [get_charts_data -model $model -name expr_model]
#echo [get_charts_data -model $model -name base_model]

set inds [filter_charts_model -model $model -expr {$AAA >= 5} -indices]
echo $inds

#set vr [process_charts_model -model $model -query -expr {$AAA >= 5}]
#echo $vr

write_charts_model -model $model -separator "\\t" -index 1

#---

#set model1 [filter_charts_model -model $model -expr {$AAA >= 5}]
#write_charts_model -model $model1 -separator "\\t" -index 1

exit
