# Y1Y2 Plots

set model1 [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set view1 [create_charts_view]
set view2 [create_charts_view]
set view3 [create_charts_view]

set plot1 [create_charts_plot -view $view1 -model $model1 -type xy -columns {{x 0} {y 1}}]
set plot2 [create_charts_plot -view $view2 -model $model1 -type xy -columns {{x 0} {y 2}}]
set plot3 [create_charts_plot -view $view3 -model $model1 -type xy -columns {{x 0} {y 2}}]

set w [qt_create_widget -type QPushButton -name Button]

qt_set_property -object $w -property text -value "Button"

set ann [create_charts_widget_annotation -plot $plot1 -id Button -widget $w]

set_charts_property -annotation $ann -name position -value {99 99 V}
set_charts_property -annotation $ann -name align -value {AlignRight|AlignTop}

place_charts_plots -view $view2 [list $plot1 $plot2 $plot3] -force

set plots [get_charts_data -view $view2 -name plots]

place_charts_plots -view $view3 $plots -force
