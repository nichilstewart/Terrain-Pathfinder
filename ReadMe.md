## Minimum Height Difference Path Visualizer

Welcome to my minimum height difference path visualizer built in OpenGL! 
The program uses a modified Dijkstra's algorithm to compute minimum paths
along a given heigth-field surface. 

### Visuals:

~To Do~

### Base Functionality:

The program reads in an image using ImageMagick and the CImg library and stores
the RGB values of each pixel to memory. The program reads the height value at
each pixel location to create 3D points, which are then used to build a series 
of "TRIANGLE_STRIP" objects that form a height map of the input image file.

A nested loop structure creates 2 points at locations (x,y) and (x,y+1) and adds
them to the currrent triangle strip. When a new point is added, it forms a 
triangular face with the last 2 verticies added to that strip. Each inner loop
adds all the points from 2 rows of points to create a strip, and each other loop
adds a new strip to the model.

The program allows users to shift, rotate, and scale the model using the mouse
and keyboard. Instructions are included below:

To transform the model:
>Press and drag the mouse button to rotate the object in any direction
Hold 'CTRL' and drag the mouse to shift the object in any direction
Hold 'SHIFT' and drag the mouse to scale the object in any direction

The program allows users to change the display mode of the model from points, to
lines, to surfaces. The view mode can be changed using input from the keyboard.
Instructions are included below:

To change the polygon display mode:
>Press '1' to view model as point cloud
Press '2' to view model as line mesh
Press '3' to view model as fill blended surface

The camera lens is set to a 45 degree angle, and the scene is displaced 3 units
away from the lens. The matrix mode for the model is set to 'MODELVIEW'. 


### Extra Credit:

For extra credit I added colors to the model vertices, changing the heightmap
colors to display as either red, green, blue, or grayscale. This can be done by
pressing the number keys on the keyboard. Instructions are included below:

To change the color display of the model:
>Press '4' to set the model color to greyscale
Press '5' to set the model color to red
Press '6' to set the model color to green
Press '7' to set the model color to blue


### Video Note:

I chose to use OBS Studio to record my video instead of using the built-in 
screenshot capture. I have left the screenshot capture in the same state it
was in the starter code. The video is included in the same directory as this 
readme file.


Thank you for looking at my program!
