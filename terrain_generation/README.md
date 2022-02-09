# Assignment 1.0.1 - Terrain Generation

By Joseph Holtkamp

## Summary

This was a fun assignment for me. I got to implement dijkstra's algorithm and do seed growth. I've never gotten to implement a pathfinding algorithm before and this gave me the perfect excuse. I use a lot of random probability to place the seeds, as well as alter the growth of them. The more seeds I added, the more natural my maps looked.

One large issue that I had when implementing this portion of the project was that I would worry too much about optimization and not on the task at hand. This would make me avoid time intensive operations even when they were necessary for the way I was doing things like the expanding of the seeds. I did not understand why Prof. Shaeffer said we needed a queue to implement seed growth, but perhaps it is more efficient than the method I utilized. Seeing project 1.02, I am glad I already have logic for if an entrance is preset as in coming from another map. 