set model [load_charts_model -csv data/Arthritis.csv -first_line_header]

#set plot [create_charts_plot -type scatter -columns {{x ID} {y Age}}]

#---

# build frequency table for Improved column
set ImprovementNames  [get_charts_data -model $model -column Improved -name details.unique_values]
set ImprovementCounts [get_charts_data -model $model -column Improved -name details.unique_counts]

set model1 [load_charts_model -tcl [list $ImprovementNames $ImprovementCounts]]

set_charts_data -model $model1 -column 0 -header -name value -value Improvement
set_charts_data -model $model1 -column 1 -header -name value -value Frequency

set plot [create_charts_plot -model $model1 -type barchart \
  -columns {{name Improvement} {values Frequency}}]

#---

# build frequency table for Treatment column
set TreatmentNames  [get_charts_data -model $model -column Treatment -name details.unique_values]
set TreatmentCounts [get_charts_data -model $model -column Treatment -name details.unique_counts]

set model2 [load_charts_model -tcl [list $TreatmentNames $TreatmentCounts]]

set_charts_data -model $model2 -column 0 -header -name value -value Treatment
set_charts_data -model $model2 -column 1 -header -name value -value Frequency

set plot [create_charts_plot -model $model2 -type barchart \
  -columns {{name Treatment} {values Frequency}}]

#---

#set model3 [create_charts_pivot_model -model $model \
#  -hcolumns {Improved} -vcolumns {Treatment} -dcolumn {Age}]

set model3 [create_charts_pivot_model -model $model \
  -hcolumns {Improved} -vcolumns {Treatment} -value_type count]

show_charts_manage_models_dlg
