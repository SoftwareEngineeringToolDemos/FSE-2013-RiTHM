#!/bin/sh

upper_bound=`java -jar jars/Solution_Generator.jar $1 $2 -greedy`

echo "Number of Vertices(Greedy Solution): " $upper_bound
 
