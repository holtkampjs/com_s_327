# Assignment 0 - The Knight's Tour

By Joseph Holtkamp

## Summary

This project was more frustrating for me than it should have been. Initially, I wrote out a solution that couldn't find any tours. I believe it was not backtracking successfully. After toying with that, I started again from scratch adding in the global variables that didn't need to be passed from function to function. When I got it to run, I was only getting 3 solutions. The way I tracked down my bug was stepping through the example solution provided in the assignment document using GDB. I discovered that my backtracking worked in all cases except for the success case. It was leaving the final landing place of the knight marked. Once I fixed that, all 1728 solutions printed out like nothing.

I added the visual representation of the knight's tours as an optional parameter. To see the basic comma separated output, simply run the executable as `./kt`. If you would like to see the printout of each final tour as a separate grid, simply add the option `v` (e.g. `./kt v`). Also my code should support larger or smaller square grids by changing the value of the `DIM` macro to represent the side length.
