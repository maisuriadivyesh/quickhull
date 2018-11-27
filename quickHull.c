#include <stdio.h>
#include <stdlib.h>

#define INFI -1               // Defult value, as we know no parameters will go negative here
#define ENABLE_DEBUG 0        // 1 to enable debugs and 0 to disable the debugs

// Declaration start here
struct point_t                  // Point structure for the points
{
  long long x, y;                      // X & Y- Coordinates of Point
};

typedef struct point_t Point;

long long convexPointCnt = 0;                           // Global counter that keeps track of number of convex hull points

int compare (const void *arg1, const void *arg2);                   // Comparator function
long long initConvexHull(Point points[], Point convexHull[], long long size);          // Initialize the convex hull logic
long long findConvexHull(Point points[], Point convexHull[], long long size, long long minPoint, long long maxPoint);                // Find one part of convex hull recuresively
long long printConvexHull(Point convexHull[]);                                                                      // Prints convex hull
long long findFurthestPointOnLeft(Point points[], long long size, long long minPoint, long long maxPoint);       // Finds furthest point on the left side of the line created by minPoint and maxPoint

long long getTotalLines(FILE *fp);        // Find out total points from given input file
long long totalLines;                     // Store total points gloablly

// Definitions start here
long long getTotalLines(FILE *fp)
{
  long long totalLines = 0;
  char string[100];

  while(!feof(fp)) {
    fgets(string, 100, fp);
    totalLines++;
  }

  totalLines--;

  if(fseek(fp, 0L, SEEK_SET) == EOF) {
    perror("Error while seeking to begining of file");
    exit(0);
  }

  return totalLines;
}

int compare (const void *arg1, const void *arg2)
{
  return (int)((( Point *)arg1)->x - (( Point *)arg2)->x);
}

long long printConvexHull(Point convexHull[])
{
  long long i = 0;

  printf("Convex Hull Points are:\n");
  for (i = 0; i < convexPointCnt; i++)
  {
    printf("(%lld, %lld)\n", convexHull[i].x, convexHull[i].y);
  }

  return 0;
}

long long initConvexHull(Point points[], Point convexHull[], long long size)
{
  long long i = 0;
  long long minPoint = 0, maxPoint = (size-1);
  //Find extreme Points
  for (i = 0; i < size; i++)
  {
    if ((points[i].x == points[minPoint].x) && (points[i].y < points[minPoint].y))      // If the X cordinate of two or more left most point same, go for lowest Y cordinate
    {
      minPoint = i;
    }

    if ((points[i].y == points[maxPoint].y) && (points[i].y > points[maxPoint].y))      // If the Y cordinate of two or more right most point same, go for highest X cordinate
    {
      maxPoint = i;
    }
  }

#if ENABLE_DEBUG
  printf("Extreme Points are Point1 : %lld and Point2: %lld\n", minPoint, maxPoint);
#endif

  // Find the convex hull recuresively
  findConvexHull(points, convexHull, size, minPoint, maxPoint);    // Find on the upper side
  findConvexHull(points, convexHull, size, maxPoint, minPoint);    // Find on the lower side

  // Print convex hull
  printConvexHull(convexHull);
  return 0;
}

long long findConvexHull(Point points[], Point convexHull[], long long size, long long minPoint, long long maxPoint)
{
  long long furthestPoint = INFI;

  furthestPoint = findFurthestPointOnLeft(points, size, minPoint, maxPoint);      // Finds the furthest point on the left side

  if (furthestPoint == INFI)                          // Current max Point is the furthest point, add it to the result
  {
    convexHull[convexPointCnt].x = points[maxPoint].x; convexHull[convexPointCnt++].y = points[maxPoint].y;
#if ENABLE_DEBUG
    printf("Furthest Point found is: (%lld, %lld)\n", points[maxPoint].x, points[maxPoint].y);
#endif
    return 0;
  }

  findConvexHull(points, convexHull, size, minPoint, furthestPoint);   // Find on the left side
  findConvexHull(points, convexHull, size, furthestPoint, maxPoint);   // Find on the right side

  return 0;
}

long long findFurthestPointOnLeft(Point points[], long long size, long long minPoint, long long maxPoint)
{
  long long furthestPoint = INFI;
  float maxDistance = 0;                  // Default to 0
  long long i = 0;
  long long minX = points[minPoint].x;
  long long minY = points[minPoint].y;
  long long maxX = points[maxPoint].x;
  long long maxY = points[maxPoint].y;
  long long canX = 0;                           // Candidate Point's X
  long long canY = 0;                           // Candidate Point's Y
  Point vector1;                   // For Cross product calculation for checking if the point is  on  the left
  Point vector2;                   // For Cross product calculation for checking if the point is  on  the left
  long long crossProduct = 0;                   // For Cross product calculation for checking if the point is  on  the left

  for (i = 0; i < size; i++)
  {
    canX = points[i].x;
    canY = points[i].y;

    vector1.x = (maxX - minX); vector1.y = (maxY - minY);
    vector2.x = (maxX - canX); vector2.y = (maxY - canY);

    crossProduct = ((vector1.x * vector2.y) - (vector1.y * vector2.x));       // if < 0, point is on the left side, if > 0, point is on the right side, if = 0, point is on the line

    if (crossProduct < 0)   // Point is on the left side
    {
#if ENABLE_DEBUG
      printf("Left side Point: (%lld, %lld) and Dist: %f and MaxDist: %f\n", canX, canY, (float)((minX*maxY) + (canX*minY) + (maxX*canY) - (canX*maxY) - (maxX*minY) - (minX*canY)), maxDistance);
#endif
      if ((float)((minX*maxY) + (canX*minY) + (maxX*canY) - (canX*maxY) - (maxX*minY) - (minX*canY)) > maxDistance)   // Based on the Distance equation taught in class
      {
        furthestPoint = i;
        maxDistance = (float)((minX*maxY) + (canX*minY) + (maxX*canY) - (canX*maxY) - (maxX*minY) - (minX*canY));
      }
    }
  }

  return furthestPoint;
}

int main (int argc, char *argv[])
{
  FILE *inputFD;
  char string[100];
  long long i = 0;

  if (argc != 2)                                                                // Make sure the number of input is correct
  {
    printf("Error! Invalid number of Arguments. Please run program as ./quickHull input.txt\n");
    return -1;
  }

  inputFD = fopen(argv[1], "r");                                                // Open file for Reading the input
  if (inputFD == NULL)
  {
    perror("Error opening the input file");
    return -1;
  }

  totalLines = getTotalLines(inputFD);
  long long size = totalLines;

  Point *points = malloc(totalLines * sizeof(Point));            // Allocate the amount of memory required to store all points
  Point *convexHull = malloc(totalLines * sizeof(Point));              // Holds the reulting convex hull points

  for (i=0; i < size; i++)
  {
    fgets(string, 100, inputFD);
    sscanf(string, "%lld %lld", &(points[i].x), &(points[i].y));
  }
  fclose(inputFD);                // Close the input file, we're done reading it

  #if ENABLE_DEBUG
    printf("Input Points:\n");
    for (i = 0; i < size; i++)
    {
      printf("Point %lld : (%lld, %lld)\n", i, points[i].x, points[i].y);
    }
  #endif

  if (size < 3)
  {
    printf("At least 3 points needed to create a convex hull\n");
    free(points);
    free(convexHull);
    return 0;
  }

  qsort(points, size, sizeof(Point), compare);     // Sort WRT to X axis

#if ENABLE_DEBUG
  printf("Sorted Points:\n");
  for (i = 0; i < size; i++)
  {
    printf("Point %lld : (%lld, %lld)\n", i, points[i].x, points[i].y);
  }
#endif

  initConvexHull(points, convexHull, size);     // Start the logic to find the convex hull

  free(points);
  free(convexHull);
  return 0;
}
