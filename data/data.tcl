set models [get_charts_data -name models]

foreach model $models {
  puts "Model: $model"
}

set views [get_charts_data -name views]

foreach view $views {
  puts "View: $view"
}

set plots [get_charts_data -name plots]

foreach plot $plots {
  puts "Plot: $plot"
}

foreach view $views {
  set plots [get_charts_data -view $view -name plots]

  foreach plot $plots {
    puts "Plot: $plot"

    set annotations [get_charts_data -plot $plot -name annotations]

    foreach annotation $annotations {
      puts "Annotation: $annotation"
    }
  }

  set annotations [get_charts_data -view $view -name annotations]

  foreach annotation $annotations {
    puts "Annotation: $annotation"
  }
}
