Project part 4 - Player characters and trainers
By Joseph Holtkamp

It is very late, I utilized the extra time on this project for other classes so I earned myself this long late work night. 
During this part of the project, I decided that I would implement ncurses because I didn't see how I could update the screen in a coherent manner otherwise. I understand that it may have been the setback that prevented me from turning it in sooner, but now I have nice colored output. 

My project is far from perfect at the moment. Occassionally, the program crashes due to segmentation faults or double frees that trace back to the findpath() > remove_heap_min() function calls. This alarms me since I did not believe I had changed it from the professor's implementation. Regardless, just try rerunning it, the crashes aren't consistent meaning it is likely caused elsewhere. To move the PC, use WASD controlls, I left the fly control with the 'f' key if someone wants to travel from map to map. At the moment, the PC cannot use the exits, they are... illusions. 

To top this off, what I'm most proud of isn't even hard, but made me happy. A big issue with this game is we need a specific area because our game will always be 80 X 25. If someone's terminal window is too small, the display will be off. So what I did was created a prescreen that tells you how big your window is and what size it needs to be. To test this, just make your terminal window smaller than the game screen and run the program. Once your terminal is the adequate minimum size you are able to proceed. 
