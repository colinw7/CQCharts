# population,fertility,HIV,CO2,BMI_male,GDP,BMI_female,life,child_mortality,Region
#set model [load_charts_model -csv data/gapminder.csv -first_line_header]

# model,mpg,cyl,disp,hp,drat,wt,qsec,vs,am,gear,carb
set model [load_charts_model -csv data/mtcars.csv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x mpg} {y wt}}]
