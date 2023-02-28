Java Swing App, uses dll written in C++ (created with Visual Studio),
for the search using filters part of the project.
Has additional custom visualization tools such as Pie chart and XY Graphs
(no libs used, based only on Spring basic components).

<br/>
<pre>
Software Functionalities (No special queries as in CLI app):
    Loading data about Olympic events, athletes, and participation records
    Parsing the collected data
    Basic data manipulation, including displaying athlete, game, and event data, as well as filtering data
    Basic data visualization, including using Pie charts and XY graphs to display data
described below:

<br/>
Basic data manipulation (done over DLLs):

<br/>
The user can view the following basic metrics:
    ● the number of participants in all events at the Olympic Games
    ● the total number of disciplines at the Olympic Games
    ● the average height of all athletes (if there is data on the height of the athlete)
    ● the average weight of all athletes (if there is data on the weight of the athlete)

<br/>
The user can filter the data when displaying the basic metrics, based on one or
more of the following criteria:
    ● filtering based on the specified sport
    ● filtering based on the year of the Olympic Games
    ● filtering based on the type of event (individual or team)
    ● filtering based on the medal won (gold, silver, bronze, or no medal)

<br/>
Data Visualization:
<br/>

The Pie chart displays the number of participations in the Olympics per country,
with the ability to filter data based on criteria listed in the previous section.

The XY chart is used to show trends of a particular data set (y-axis) in relation to the year of the
Olympic games (x-axis). Users can select a range of years for data display.
Data is shown in equal intervals on both axes.
The data should be shown separately for summer Olympics (in red) and winter Olympics (in blue).
On the y-axis, the following metrics can be displayed:
    ● Total number of disciplines
    ● Average height of all athletes
    ● Average weight of all athletes

</pre>
