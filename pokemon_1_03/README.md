# Assignment 1.03 - Joseph Holtkamp

For this assignment, I looked through the description and evaluated the code I had previously written and determined that it may be best for me to apply the professor's code to my own. After reading through some of the provided solution code, I decided it was best to use it as a starting point rather than my own. This came from a combination of incompatability of my code with the professor's and a higher level of trust in the professor's implementation than my own for the time being. After some time examining the provided code for 1.02, it became aparent to me that I was not going to understand the provided code piece by piece. Instead, I decided to read the code from the top to the bottom. By doing that, I was able to focus enough to break down the provided code to understand it.

After which, I determined that the entry point to solving the provided problem. I replicated the code for dijkstra_path and made a few choice alterations. First, there isn't a destination so I removed the to pair. Second, the cost of a move doesn't depend on height so I wrote a conditional statement to determine the cost based on the trainer and terrain types. Then, each of the 8 immediately availabe squares are checked if they are closer than previously determined and updated if so. Lastly, rather than ending when first reached, to determine the weights, all nodes needed to be visited at least once.

My output is the map followed by the weights for a hiker, rival, pc, and other, respectively. After that, it accepts map moving input in the same way as the previous assignment (1.02). I display a player as the '@' symbol which is randomly placed each iteration. At the moment, there is no real logic to the placement of the player other than showing on the map where it'll be.