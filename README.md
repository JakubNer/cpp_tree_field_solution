This is an update on someone else's solution to a fun problem.

A certain group of devs were challenging themselves to create the fastest solution to a problem.  

I didn't come up with my own solution, just made one of the other dev's solution faster.

# Original Problem

Given a “Field of Trees”, find the largest rectangle inside it that has no trees.

The field is X x X.

There are X trees scattered around the field ( but not touching the edge ).

You want to build a house ( or a soccer field, or something ) without cutting down any trees.

Find the largest rectangle ( by area ) that you can fit between the trees.

# Original Solution

The original solution by mahmadij@github:

https://github.com/mahmadij/cpp_tree_field_solution

# My Hacks

My hacks to the solution included.

  - cleanup of headers
  - using stdlib qsort instead of a custom bubble sort
  - using four threads instead of one (although I still resorted the whole field in each thread)
  
For each run I output the result of the original, qsorted one thread solution, and the four thread solution.

