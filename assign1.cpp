// assign1.cpp : Defines the entry point for the console application.
//

/*
  CSCI 420 Computer Graphics
  Assignment 1: Height Fields
  By: Nichil Stewart
*/


#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "CImg-2.3.5\CImg.h"
#include <iostream>
#include "pic.h"
#include <algorithm> 
#include <vector>
#include <set>



using namespace cimg_library;

int g_iMenuId;

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = { 0.0, 0.0, 0.0 };
float g_vLandTranslate[3] = { 0.0, 0.0, 0.0 };
float g_vLandScale[3] = { 1.0, 1.0, 1.0 };

CImg<unsigned char>* g_pHeightData;

char colormode = 'N'; //sets the display color value, currently set to greyscale

/* This line is required for CImg to be able to read jpg/png format files. */
/* Please install ImageMagick and replace the path below to the correct path to convert.exe on your computer */
void initializeImageMagick()
{
    cimg::imagemagick_path("C:\\Program Files\\ImageMagick-7.0.10-Q16-HDRI\\convert.exe", true); //path fix
}


/* Write a screenshot to the specified filename */
void saveScreenshot(char* filename)
{
    //Note: I used OBS Studio to record my video and haven't used this code

    int i, j;

    if (filename == NULL)
        return;

    /* Allocate a picture buffer */
    CImg<unsigned char> in(640, 480, 1, 3, 0);

    printf("File to save to: %s\n", filename);

    for (i = 479; i >= 0; i--) {
        glReadPixels(0, 479 - i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
            in.data());
    }

    if (in.save_jpeg(filename))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");
}

void myinit()
{
    /* setup gl view here */
    glClearColor(0.0, 0.0, 0.0, 0.0); //clear the color vals
    glEnable(GL_DEPTH_TEST); //enable depth test so objects appear in correct order
    glShadeModel(GL_SMOOTH); //make it smooth!
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //supposed to improve blending
    glEnable(GL_BLEND); //supposed to improve blending
}



                                                               ///////////////// Perform Dijkstra's algorithm here! /////////////////////////

int imageWidth = 0; //global image width for Dijkstra's implementation!
int imageHeight = 0; //global image height for Dijkstra's implementation!

int shortest(int** grid);

void setArrayDimensions() {
    imageWidth = g_pHeightData->width(); //get image width
    imageHeight = g_pHeightData->height(); //get image height
}

void deleteMem(int ** imagearray) { //REMEMBER TO DELETE THE MEMORY!!!
    for (int i = 0; i < imageHeight; i++) {
        delete[] imagearray[i];
    }
    delete[]imagearray;
}

void convertToArray() { //create int[][] from existing height data!

    setArrayDimensions(); //set image dimensions
    if (imageWidth==0 || imageHeight==0) { //if either = 0, return
        std::cout << "array dimension set to 0" << std::endl;
        return;
    }

    int** imagearray = new int* [imageHeight];
    for (int i = 0; i < imageHeight; ++i) {
        imagearray[i] = new int[imageWidth];
    }

    //std::cout << "Converting to Array: " << std::endl;

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            unsigned char* h1 = g_pHeightData->data(x, y); //get height of first pixel
                                                                                                       //Currently getting int values from 1 -> 255, might need to adjust range??
            //float p1 = (float)*h1 / 255.0f; //height for top point
            float p1 = (float)*h1; //height for top point
            int depth_val = static_cast<int>(p1);
            imagearray[y][x] = depth_val;
            //std::cout << imagearray[y][x] << " ";
        }
        //std::cout << std::endl;
    }

    shortest(imagearray);
                                                                                                        //make sure to save paths array!!!
    deleteMem(imagearray);
}



struct cell {
    int x, y;
    int distance;
    cell(int x, int y, int distance) :
        x(x), y(y), distance(distance) {}
    cell() {}
};

std::vector<cell> path;                                                                                     //(CELL ARRAY WE USE FOR FINAL PATH ARRAY!

bool operator<(const cell& a, const cell& b)
{
    if (a.distance == b.distance) {
        if (a.x != b.x)
            return (a.x < b.x);
        else
            return (a.y < b.y);
    }
    return (a.distance < b.distance);
}

bool isInsideGrid(int i, int j) {
    int ROW = imageHeight;
    int COL = imageWidth;
    return (i >= 0 && i < COL&& j >= 0 && j < ROW);
}

void deleteMem2(int** dis, cell** parent) { //REMEMBER TO DELETE THE MEMORY!!!
    
    for (int i = 0; i < imageHeight; i++) {
        delete[] dis[i];
    }
    delete[]dis;

    for (int i = 0; i < imageHeight; i++) {
        delete[] parent[i];
    }
    delete[]parent;
}

//int shortest(int grid[ROW][COL], int row, int col) {
int shortest(int ** grid) {
    int row = imageHeight;
    int col = imageWidth;

    /*std::cout << "Inside vals: " << std::endl;
    for (int i = 0; i < row; i++, std::cout << std::endl) {
        for (int j = 0; j < col; j++) {
            std::cout << grid[i][j] << " ";
        }
    }
    std::cout << std::endl << "Done: " << std::endl << std::endl;*/



    int** dis = new int* [imageHeight];
    for (int i = 0; i < imageHeight; ++i) {
        dis[i] = new int[imageWidth];
        for (int j = 0; j < imageWidth; j++) {
            dis[i][j] = INT_MAX;
        }
    }

    cell** parent = new cell* [imageHeight];
    for (int i = 0; i < imageHeight; ++i) {
        parent[i] = new cell[imageWidth];
    }

    int dx[] = { -1, 0, 1, 0 };
    int dy[] = { 0, 1, 0, -1 };

    std::set<cell> st;
    st.insert(cell(0, 0, 0));

    //dis[0][0] = grid[0][0];  
    dis[0][0] = 0;

    while (!st.empty())
    {
        cell k = *st.begin();
        st.erase(st.begin());

        // looping through all neighbours 
        //POTENTIALLY ADD DIAGONALS??
        for (int i = 0; i < 4; i++)
        {
            int x = k.x + dx[i];
            int y = k.y + dy[i];

            if (!isInsideGrid(x, y))
                continue;
                                                                                //DOES NOT CURRENTLY ADD WEIGHT FOR STEPPING FROM ONE VALUE TO ANOTHER. (NEED TO ADD TO MINIMIZE STEPS)
            if (dis[x][y] > dis[k.x][k.y] + abs(grid[k.x][k.y] - grid[x][y]))   //abs(grid[k.x][k.y] - grid[x][y]) is the added weight between two locations!
            {
                if (dis[x][y] != INT_MAX) {
                    st.erase(st.find(cell(x, y, dis[x][y])));
                }

                dis[x][y] = dis[k.x][k.y] + abs(grid[k.x][k.y] - grid[x][y]);

                parent[x][y].x = k.x;
                parent[x][y].y = k.y;
                parent[x][y].distance = grid[k.x][k.y]; //depth (z) value!!


                st.insert(cell(x, y, dis[x][y]));
            }
        }
    }

    //std::vector<cell> path;
    cell lastcell(row - 1, col - 1, grid[row - 1][col - 1]);
    path.push_back(lastcell);

    cell k = parent[row - 1][col - 1];
    while (k.x != 0 || k.y != 0) {
        path.push_back(k);
        k = parent[k.x][k.y];
    }

    cell firstcell(0, 0, grid[0][0]);
    path.push_back(firstcell);

    std::reverse(path.begin(), path.end());
    
    std::cout << "Shortest path: " << std::endl;
    for (int i = 0; i < path.size(); i++) {
        std::cout << "(" << path[i].x << ", " << path[i].y << ", " << path[i].distance << ")" << std::endl;
    }

    /*for (int i = 0; i < row; i++, std::cout << std::endl) {
        for (int j = 0; j < col; j++) {
            std::cout << dis[i][j] << " ";
        }
    }*/

    int returnval = dis[row - 1][col - 1];
                                                                                                        //delete memory from dis[][] and cell[][]
    deleteMem2(dis, parent);
    return returnval;
    //return dis[row - 1][col - 1];
}


void CreateTStrips()
{
    int width = g_pHeightData->width(); //get image width
    int height = g_pHeightData->height(); //get image height

    for (int y = 0; y < height - 1; y++) {
        glBegin(GL_TRIANGLE_STRIP); //create new strip at beginning of row
        for (int x = 0; x < width; x++) {
            //will build top and bottom points, then shift right building next 2 points
            //will use last 2 points to continue connecting the triangle strip to new vertex
            
            unsigned char* h1 = g_pHeightData->data(x, y); //get height of first pixel
            unsigned char* h2 = g_pHeightData->data(x, y + 1); //get height of second pixel

            y -= height / 2; x -= width / 2; //push all x and y values to left by half of width

            float p1 = (float)*h1 / 255.0f; //height for top point
            float p2 = (float)*h2 / 255.0f; //height for bottom point
            float col1_r = p1, col1_g = p1, col1_b = p1; //r,g,b variables for height1
            float col2_r = p2, col2_g = p2, col2_b = p2; //r,g,b variables for height2

            if (colormode == 'R') { //turn to red
                col1_r = 1.0;
                col2_r = 1.0;
            }
            else if (colormode == 'G') { //turn to green
                col1_g = 1.0;
                col2_g = 1.0;
            }
            else if (colormode == 'B') { //turn to blue
                col1_b = 1.0;
                col2_b = 1.0;
            }

            glColor3f(col1_r, col1_g, col1_b); //set colors using above changes
            glVertex3f(x / (float)width, y / (float)height, -p1/4.0f); //z value changed to not break the object
            glColor3f(col2_r, col2_g, col2_b); //set colors using above changes
            glVertex3f(x / (float)width, (y + 1) / (float)height, -p2/4.0f); //z value changed to not break the object

            y += height / 2; x += width / 2; //undo change to x and y values
        }
        glEnd(); //end strip after full loop through row 
    }

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < path.size(); i++) {
        glColor3f(1.0f, 0.0f, 0.0f); //set colors using above changes
        //glVertex3d(path[i].x, path[i].y, path[i].distance);
        //glVertex3d((((float)path[i].x) - width/2)/width, (((float)path[i].y) - height/2 )/height, -((float)path[i].distance)/(255.0f*4.0f));
        glVertex3d((((float)path[i].y) - height / 2) / height, (((float)path[i].x) - width / 2) / width, -((float)path[i].distance) / (255.0f * 4.0f));

        //FLIPPED X AND Y VARIABLES IN MY PATHS FUNCTION!!! 
        //NOT SURE HOW THIS MAY IMPACT NON-SQUARE HEIGHT MAPS?!?
    }
    glEnd();
}


                                                                                            ////////ASSIGNMENT 2 PROGRESS GOES HERE///////////////
/* represents one control point along the spline */
struct point {
    double x;
    double y;
    double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
    int numControlPoints;
    struct point* points;
};

/* the spline array */
struct spline* g_Splines;
/* total number of splines */
int g_iNumOfSplines;

std::vector<point> divided_points;
int currPointIndex = 0;

int loadSplines() {
    //load all spline values into g_Splines

    g_iNumOfSplines = 1; //TOTAL # OF SPLINES (START WITH 1??)
    g_Splines = (struct spline*)malloc(g_iNumOfSplines * sizeof(struct spline));

    int iLength = path.size();

    for (int i = 0; i < g_iNumOfSplines; i++) {
        g_Splines[i].points = (struct point*)malloc(iLength * sizeof(struct point));
        g_Splines[i].numControlPoints = iLength;

        for (int j = 0; j < path.size(); j++) {

            //x:    (value) - height / 2) / height
            //y:    (value) - width / 2) / width 
            //z:    -(value) / (255.0f * 4.0f)

            //g_Splines[i].points[j].x = (double)path[j].x;
            //g_Splines[i].points[j].y = (double)path[j].y;
            //g_Splines[i].points[j].z = (double)path[j].distance;

            g_Splines[i].points[j].x = (((double)path[j].x) - imageHeight/2) / imageHeight;
            g_Splines[i].points[j].y = (((double)path[j].y) - imageWidth/2) / imageWidth;
            g_Splines[i].points[j].z = -((double)path[j].distance) / (255.0f * 4.0f);
        }

    }

    return 0;
}

//point calcPoint(double t, point* c_pts) {
point calcPoint(double t, point p0, point p1, point p2, point p3) {

    // For: q(t) = 0.5 * (1.0f,t,t2,t3)  *	
    // [ 0  2  0  0 ] [P0]
    // [-1  0  1  0 ] [P1]
    // [ 2 -5  4 -1 ] [P2]
    // [-1  3 -3  1 ] [P3]
    // In C:   		  (1.0f)          (t)                     (t^2)									  (t^3)
    // q(t) = 0.5 * ((2 * P1) + (-P0 + P2) * t + (2 * P0 - 5 * P1 + 4 * P2 - P3) * t2 + (-P0 + 3 * P1 - 3 * P2 + P3) * t3)

    //Note: c_pts for control points array
    point curr_pt; //curr point to return

    curr_pt.x = 0.5 * ((2 * p1.x)
        + (-p0.x + p2.x) * t
        + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * pow(t, 2)
        + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * pow(t, 3));

    curr_pt.y = 0.5 * ((2 * p1.y)
        + (-p0.y + p2.y) * t
        + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * pow(t, 2)
        + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * pow(t, 3));

    curr_pt.z = 0.5 * ((2 * p1.z)
        + (-p0.z + p2.z) * t
        + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * pow(t, 2)
        + (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * pow(t, 3));

    return curr_pt;
}

double calcDistance(point p0, point p1) {
    return sqrt(pow((p1.x - p0.x), 2) + pow((p1.y - p0.y), 2) + pow((p1.z - p0.z), 2));
}

//point calcTangent(double t, point* c_pts) { //For camera!, set t to 0.005f		
point calcTangent(double t, point p0, point p1, point p2, point p3) { //For camera!, set t to 0.005f													
    //0.5 * ((-P0 + P2) + (2 * P0 - 5 * P1 + 4 * P2 - P3) * (2 * t) + (-P0 + 3 * P1 - 3 * P2 + P3) * (3 * t^2));

    point tang; t = 0.005;

    tang.x = 0.5 * ((-p0.x + p2.x)
        + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * (2 * t)
        + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * (3 * t * t));

    tang.y = 0.5 * ((-p0.y + p2.y)
        + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * (2 * t)
        + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * (3 * t * t));

    tang.z = 0.5 * ((-p0.z + p2.z)
        + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * (2 * t)
        + (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * (3 * t * t));

    double abs_normal = sqrt(pow(tang.x, 2) + pow(tang.y, 2) + pow(tang.z, 2)); //convert to normal
    tang.x /= abs_normal;
    tang.y /= abs_normal;
    tang.z /= abs_normal;

    return tang;
}

point calcNormal(point tang) {
    point norm;
    norm.x = -(tang.z * 1);
    norm.y = 0;
    norm.z = tang.x * 1;
    return norm;
}

point calcBinormal(point norm, point tang) {
    //compute crossproduct of normal and tangent for binormal
    point binorm;
    binorm.x = norm.y * tang.z - norm.z * tang.y;
    binorm.y = norm.z * tang.x - norm.x * tang.z;
    binorm.z = norm.x * tang.y - norm.y * tang.z;
    return binorm;
}

void setView(int i) { //i for index in points array

    point p0 = divided_points[i];
    point p1 = divided_points[i + 1];
    point p2 = divided_points[i + 2];
    point p3 = divided_points[i + 3];

    point tang = calcTangent(0.005, p0, p1, p2, p3);
    point norm = calcNormal(tang);
    point binorm = calcBinormal(norm, tang);

    //std::cout << i << " x vals: " << tang.x << " " << norm.x << " " << binorm.x << std::endl;
    //std::cout << i << " y vals: " << tang.y << " " << norm.y << " " << binorm.y << std::endl;
    //std::cout << i << " z vals: " << tang.z << " " << norm.z << " " << binorm.z << std::endl;

    /*gluLookAt(p0.x, p0.y + 0.1f, p0.z,
        p0.x + tang.x, p0.y + tang.y + 0.1f, p0.z + tang.z,
        binorm.x, binorm.y, binorm.z
    );*/

    gluLookAt(p0.x, p0.y + 0.1f, p0.z,
        p0.x + tang.x, p0.y + tang.y + 0.1f, p0.z + tang.z,
        binorm.x, binorm.y, binorm.z
    );

    return;
}


void makeSplines() {
    for (int i = 0; i < g_iNumOfSplines; i++) {
        for (int j = 0; j < g_Splines[i].numControlPoints - 3; j++) {

            point p0 = g_Splines[i].points[j];
            point p1 = g_Splines[i].points[j + 1];
            point p2 = g_Splines[i].points[j + 2];
            point p3 = g_Splines[i].points[j + 3];

            for (double t = 0; t < 1.0; t += 0.005) {

                point curr = calcPoint(t, p0, p1, p2, p3);
                divided_points.push_back(curr);
            }
        }
    }
}


                                                                                                ////////PROGRESS ENDS HERE


void UpdateRenderView(unsigned char key, int x, int y) {
    switch (key) {
    case '1': //point view
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case '2': //line view
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case '3': //fill view
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case '4': //no color
        colormode = 'N';
        break;
    case '5': //red color
        colormode = 'R';
        break;
    case '6': //blue color
        colormode = 'G';
        break;
    case '7': //green color
        colormode = 'B';
        break;
    default: //fill as standard
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}


void display()
{
    int width = g_pHeightData->width(); //get image width
    int height = g_pHeightData->height(); //get image height

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear colors/depth buffer
    
                                                    //glLoadIdentity(); //load identity
                                                    //gluPerspective(45.0f, (1.0f * width) / height, 0.01f, 1000.0f); //45 degree perspective view
                                                    //glMatrixMode(GL_MODELVIEW); //set up model view

                                                    //glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2] - 3.0f); //3.0 is a nice distance!
                                                    //glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]); //scale by x, y, z vals
                                                    //glRotatef(g_vLandRotate[0], 1.0f, 0.0f, 0.0f); //x dir rotate
                                                    //glRotatef(g_vLandRotate[1], 0.0f, 1.0f, 0.0f); //y dir rotate
                                                    //glRotatef(g_vLandRotate[2], 0.0f, 0.0f, 1.0f); //z dir rotate
                                                    //glRotatef(180.0f, 1.0f, 0.0f, 0.0f); //backwards unless flipped..

    CreateTStrips(); //call create function above

    //NEW STUFF

    point p0 = divided_points[currPointIndex];
    point p1 = divided_points[currPointIndex + 1];
    point p2 = divided_points[currPointIndex + 2];
    point p3 = divided_points[currPointIndex + 3];

    point tan = calcTangent(0.005, p0, p1, p2, p3);
    point normal = calcNormal(tan);

    //point normal = calcNormal(calcTangent(0.005, pts));
    normal.x *= 0.1;
    normal.y *= 0.1;
    normal.z *= 0.1;

    //setGround();
    //setSky();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < divided_points.size() - 1; i++) {
        point curr = divided_points[i];
        point next = divided_points[i + 1];

        if (abs(next.x - curr.x) < 0.1 && abs(next.y - curr.y) < 0.1 && abs(next.z - curr.z) < 0.1) { //check next point is part of current spline
            glVertex3d(curr.x + normal.x, curr.y, curr.z + normal.z);
        }
        else { //end current spline if it is not and start a new one
            glEnd();
            glBegin(GL_LINE_STRIP);
        }
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < divided_points.size() - 1; i++) {
        point curr = divided_points[i];
        point next = divided_points[i + 1];

        if (abs(next.x - curr.x) < 0.1 && abs(next.y - curr.y) < 0.1 && abs(next.z - curr.z) < 0.1) { //check next point is part of current spline
            glVertex3d(curr.x - normal.x, curr.y, curr.z - normal.z);

        }
        else { //end current spline if it is not and start a new one
            glEnd();
            glBegin(GL_LINE_STRIP);
        }

        //glVertex3d(curr.x - normal.x, curr.y, curr.z - normal.z);

    }
    glEnd();

    //setCrossSection(normal);



    //ARE THESE ALL REDUNDANT WITH ABOVE??

    glMatrixMode(GL_MODELVIEW); //set up model view
    glLoadIdentity(); //load identity
    gluPerspective(45.0f, (1.0f * width) / height, 0.01f, 1000.0f); //45 degree perspective view

    glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]); //scale by x, y, z vals
    glRotatef(g_vLandRotate[0], 1.0f, 0.0f, 0.0f); //x dir rotate
    glRotatef(g_vLandRotate[1], 0.0f, 1.0f, 0.0f); //y dir rotate
    glRotatef(g_vLandRotate[2], 0.0f, 0.0f, 1.0f); //z dir rotate
    glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);

    //setView(currPointIndex); //update camera!!

    if (currPointIndex + 1 < divided_points.size() - 3) {
        currPointIndex += 1;
    }

    glutSwapBuffers(); //swap buffers
}

void menufunc(int value)
{
    switch (value)
    {
    case 0:
        exit(0);
        break;
    }
}

void doIdle()
{
    /* do some stuff... */
    /* make the screen update */
    glutPostRedisplay();
}

/* converts mouse drags into information about
rotation/translation/scaling */
void mousedrag(int x, int y)
{
    int vMouseDelta[2] = { x - g_vMousePos[0], y - g_vMousePos[1] };

    switch (g_ControlState)
    {
    case TRANSLATE:
        if (g_iLeftMouseButton)
        {
            g_vLandTranslate[0] += vMouseDelta[0] * 0.01;
            g_vLandTranslate[1] -= vMouseDelta[1] * 0.01;
        }
        if (g_iMiddleMouseButton)
        {
            g_vLandTranslate[2] += vMouseDelta[1] * 0.01;
        }
        break;
    case ROTATE:
        if (g_iLeftMouseButton)
        {
            g_vLandRotate[0] += vMouseDelta[1];
            g_vLandRotate[1] += vMouseDelta[0];
        }
        if (g_iMiddleMouseButton)
        {
            g_vLandRotate[2] += vMouseDelta[1];
        }
        break;
    case SCALE:
        if (g_iLeftMouseButton)
        {
            g_vLandScale[0] *= 1.0 + vMouseDelta[0] * 0.01;
            g_vLandScale[1] *= 1.0 - vMouseDelta[1] * 0.01;
        }
        if (g_iMiddleMouseButton)
        {
            g_vLandScale[2] *= 1.0 - vMouseDelta[1] * 0.01;
        }
        break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        g_iLeftMouseButton = (state == GLUT_DOWN);
        break;
    case GLUT_MIDDLE_BUTTON:
        g_iMiddleMouseButton = (state == GLUT_DOWN);
        break;
    case GLUT_RIGHT_BUTTON:
        g_iRightMouseButton = (state == GLUT_DOWN);
        break;
    }

    switch (glutGetModifiers())
    {
    case GLUT_ACTIVE_CTRL:
        g_ControlState = TRANSLATE;
        break;
    case GLUT_ACTIVE_SHIFT:
        g_ControlState = SCALE;
        break;
    default:
        g_ControlState = ROTATE;
        break;
    }

    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

int main(int argc, char* argv[])
{
    // I've set the argv[1] to spiral.jpg.
    // To change it, on the "Solution Explorer",
    // right click "assign1", choose "Properties",
    // go to "Configuration Properties", click "Debugging",
    // then type your texture name for the "Command Arguments"
    if (argc < 2)
    {
        printf("usage: %s heightfield.jpg\n", argv[0]);
        exit(1);
    }

    initializeImageMagick();

    g_pHeightData = new CImg<unsigned char>((char*)argv[1]);
    if (!g_pHeightData)
    {
        printf("error reading %s.\n", argv[1]);
        exit(1);
    }

    convertToArray();

                                                                                                                        //get shortest path array here!!

    loadSplines();
    makeSplines();

    glutInit(&argc, (char**)argv);

    /*
        create a window here..should be double buffered and use depth testing
        the code past here will segfault if you don't have a window set up....
        replace the exit once you add those calls.
    */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA); //initialize display mode
    glutInitWindowSize(640, 480); //set display resolution
    glutInitWindowPosition(0, 0); //set display position
    glutCreateWindow("heightfield"); //create window named "heightfield"

    /* tells glut to use a particular display function to redraw */
    glutDisplayFunc(display);

    /* allow the user to quit using the right mouse button menu */
    g_iMenuId = glutCreateMenu(menufunc);
    glutSetMenu(g_iMenuId);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* replace with any animate code */
    glutIdleFunc(doIdle);

    /* callback for mouse drags */
    glutMotionFunc(mousedrag);
    /* callback for idle mouse movement */
    glutPassiveMotionFunc(mouseidle);
    /* callback for mouse button changes */
    glutMouseFunc(mousebutton);
    /* callback for keyboard button changes */
    glutKeyboardFunc(UpdateRenderView);

    /* do initialization */
    myinit();

    glutMainLoop();
    return 0;
}