## Minimum Slope Terrain Pathfinder

Welcome to my minimum height difference path visualizer built in OpenGL! 

The program uses a modified Dijkstra's algorithm to compute paths of minimum 
change in height along a given 3D heigth-field surface. Each of the paths are 
then iteratively rendered onto the height-field. The program uses a modifiable
3D camera with abilities to change the shape, color, and representation of the
field. 

### Algorithm: 

Uses a modified Dijkstra's algorithm to compute the shortest path from (0,0) 
to all other pixel positions in (1, 1) to (255,255) range. Each pixel on the
2D image file is treated as a graph node, where the difference in RGB-greyscale
value between two nodes is interpreted as the edge weight between the two nodes.

Current algorithm creates edges in 4 directions (Left+Right+Up+Down), and does 
not add a weight penalty for moving between two arbitrary adjacent edges. 

##### For v1.1: 
Testing adjacent weight penalty to give preference to paths with fewer steps.

Testing diagonal edges (with added hypotenuse weight penalty)
  
### Visuals:

Subset of least-incline paths with height-map rendered as triangle strips
![Image 1](https://raw.githubusercontent.com/nichilstewart/Min-Difference-Path-OpenGL/master/imgs/standard1.png)

Subset of least-incline paths with height-map rendered as points
![Image 2](https://raw.githubusercontent.com/nichilstewart/Min-Difference-Path-OpenGL/master/imgs/standard2.png)

Overlayed paths with height-map rendered as triangle strips
![Image 3](https://raw.githubusercontent.com/nichilstewart/Min-Difference-Path-OpenGL/master/imgs/different1.png)

Overlayed paths with height-map rendered as points
![Image 4](https://raw.githubusercontent.com/nichilstewart/Min-Difference-Path-OpenGL/master/imgs/different2.png)

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


### Additional Features:

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
