set df1 {{"A": ["A0", "A1", "A2", "A3"],
          "B": ["B0", "B1", "B2", "B3"],
          "C": ["C0", "C1", "C2", "C3"],
          "D": ["D0", "D1", "D2", "D3"]}}
set df2 {{"A": ["A4", "A5", "A6", "A7"],
          "B": ["B4", "B5", "B6", "B7"],
          "C": ["C4", "C5", "C6", "C7"],
          "D": ["D4", "D5", "D6", "D7"]}}
set df3 {{"A": ["A8", "A9", "A10", "A11"],
          "B": ["B8", "B9", "B10", "B11"],
          "C": ["C8", "C9", "C10", "C11"],
          "D": ["D8", "D9", "D10", "D11"]}}
set df4 {{"B": ["B2", "B3", "B6", "B7"],
          "D": ["D2", "D3", "D6", "D7"],
          "F": ["F2", "F3", "F6", "F7"]}}

set model1 [load_charts_model -json @df1]
write_charts_model -model $model1
set model2 [load_charts_model -json @df2]
write_charts_model -model $model2
set model3 [load_charts_model -json @df3]
write_charts_model -model $model3

#set model4 [join_charts_model -models [list $model1 $model2 $model3] -type concat]
#write_charts_model -model $model4

set model5 [join_charts_model -models [list $model1 $model2 $model3] \
  -type concat -keys [list x y z]]
write_charts_model -model $model5

set model6 [join_charts_model -models [list $model1 $model2 $model3] -axis h \
  -type concat -keys [list x y z]]
write_charts_model -model $model6

#---

exit
