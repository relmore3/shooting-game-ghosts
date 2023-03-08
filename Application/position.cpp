/*____________________________________________________________________
|
| File: position.cpp
|
| Description: Functions to create and manipulate a camera.
|
| Functions: Position_Init
|            Position_Free
|            Position_Set_Speed
|            Position_Update
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*___________________
|
| Include Files
|__________________*/

#include <first_header.h>
#include <math.h>

#include "dp.h"

#include "position.h"

/*___________________
|
| Constants
|__________________*/

#define ROTATE_UP_MAX   ((float)-89)
#define ROTATE_DOWN_MAX ((float)89)                   

#define CAMERA_DISTANCE 10	// distance of 'to' point away from the camera position

/*___________________
|
| Global variables
|__________________*/

static gx3dVector current_position;			// current position
static gx3dVector start_heading;				// start heading (normalized)
static gx3dVector current_heading;			// current heading (normalized)
static float      current_speed;				// current move speed
static float      current_xrotate;			// current rotation of camera	
static float	    current_yrotate;

/*____________________________________________________________________
|
| Function: Position_Init
|
| Input: Called from ____
| Output:
|___________________________________________________________________*/
 
void Position_Init (
  gx3dVector *position, 
  gx3dVector *heading,      // 0,0,1 for cubic environment mapping to work correctly (why?)
  float       move_speed )  // move speed in feet per second
{
  bool b;
  gx3dVector v;

  // Init global variables
  current_position = *position;
  current_heading  = *heading;
  gx3d_NormalizeVector (&current_heading, &current_heading); // just in case its not already normalized
  start_heading    = current_heading;
  current_speed    = move_speed;
  current_xrotate  = 0;
  current_yrotate  = 0;

  Position_Update (0, 0, 0, 0, true, &b, &b, &v, &v);	// force an update to start the camera off in the correct position
}

/*____________________________________________________________________
|
| Function: Position_Free
|
| Input: Called from ____
| Output:
|___________________________________________________________________*/
 
void Position_Free ()
{

}

/*____________________________________________________________________
|
| Function: Position_Set_Speed
|
| Input: Called from ____
| Output: Sets new move speed.
|___________________________________________________________________*/

void Position_Set_Speed (float move_speed)
{
  current_speed = move_speed;
}

/*____________________________________________________________________
|
| Function: Position_Update
|
| Input: Called from ____
| Output:
|___________________________________________________________________*/

void Position_Update (
  unsigned    elapsed_time,
  unsigned    move,
  int         xrotate,
  int         yrotate,
  bool        update_all,               
  bool       *position_changed, // returns true if position has changed, else false
  bool       *camera_changed,   // return true if heading has changed
  gx3dVector *new_position,
  gx3dVector *new_heading )
{
	int n;
	float move_amount;
  gx3dMatrix m, mx, my, mxy;
  gx3dVector to, world_up = { 0, 1, 0 };
	gx3dVector v1, v_right;

/*____________________________________________________________________
|
| Init variables
|___________________________________________________________________*/

  *position_changed = false;
  *camera_changed   = false;

	// Compute amount of movement to make, if any
	move_amount = ((float)elapsed_time / 1000) * current_speed;

/*____________________________________________________________________
|
| Rotate heading?
|___________________________________________________________________*/

  // Smooth out the rotations
	n = xrotate;
	xrotate = (int) sqrt ((double)(abs(xrotate)));
	if (n < 0)
		xrotate = -xrotate;
	n = yrotate;
	yrotate = (int) sqrt ((double)(abs(yrotate)));
	if (n < 0)
		yrotate = -yrotate;

	// Add to the current x axis rotation
	current_xrotate += (float)xrotate * 0.5;	// scale by .5 so doesn't rotate so fast
  if (current_xrotate < ROTATE_UP_MAX)
    current_xrotate = ROTATE_UP_MAX;
  else if (current_xrotate > ROTATE_DOWN_MAX)
    current_xrotate = ROTATE_DOWN_MAX;

	// Add to the current y axis rotation
  current_yrotate += (float)yrotate * 0.5;	// scale by .5 so doesn't rotate so fast
  while (current_yrotate < -360)
    current_yrotate += 360;
  while (current_yrotate > 360)
    current_yrotate -= 360;

  // Rotate heading
  if ((xrotate != 0) OR (yrotate != 0)) {
    gx3d_GetRotateXMatrix (&mx, current_xrotate);
    gx3d_GetRotateYMatrix (&my, current_yrotate);
    gx3d_MultiplyMatrix (&mx, &my, &mxy);
    gx3d_MultiplyVectorMatrix (&start_heading, &mxy, &current_heading);  
    // Make sure heading is normalized
    gx3d_NormalizeVector (&current_heading, &current_heading);
  }
  
/*____________________________________________________________________
|
| Move position?
|___________________________________________________________________*/
  
  if (move OR update_all) {
		if (move & POSITION_MOVE_FORWARD) {
			// Move 0.5 feet along the view vector
			gx3d_MultiplyScalarVector (move_amount, &current_heading, &v1);
		  gx3d_AddVector (&current_position, &v1, &current_position);
		}
		if (move & POSITION_MOVE_BACK) {
			// Move -0.5 feet along the view vector
			gx3d_MultiplyScalarVector (-move_amount, &current_heading, &v1);
		  gx3d_AddVector (&current_position, &v1, &current_position);
		}
		if (move & POSITION_MOVE_RIGHT) {
			// Compute the normalized right vector
			gx3d_VectorCrossProduct (&world_up, &current_heading, &v_right);
			gx3d_NormalizeVector (&v_right, &v_right);
			// Move 0.5 feet along the right vector
			gx3d_MultiplyScalarVector (move_amount, &v_right, &v1);
		  gx3d_AddVector (&current_position, &v1, &current_position);
		}
		if (move & POSITION_MOVE_LEFT) {
			// Compute the normalized right vector
			gx3d_VectorCrossProduct (&world_up, &current_heading, &v_right);
			gx3d_NormalizeVector (&v_right, &v_right);
			// Move -0.5 feet along the right vector
			gx3d_MultiplyScalarVector (-move_amount, &v_right, &v1);
		  gx3d_AddVector (&current_position, &v1, &current_position);
		}
		*position_changed = true;
	}

/*____________________________________________________________________
|
| Update camera
|___________________________________________________________________*/
  
  if ((xrotate != 0) OR (yrotate != 0) OR *position_changed) {
    // Compute a point the camera is looking at
		gx3d_MultiplyScalarVector (CAMERA_DISTANCE, &current_heading, &v1);
	  gx3d_AddVector (&current_position, &v1, &to);
//		to.x = current_position.x + (current_heading.x * CAMERA_DISTANCE);
//    to.y = current_position.y + (current_heading.y * CAMERA_DISTANCE);
//    to.z = current_position.z + (current_heading.z * CAMERA_DISTANCE);
    // Set new camera	position
	  gx3d_ComputeViewMatrix (&m, &current_position, &to, &world_up);
  	gx3d_SetViewMatrix (&m);
    *camera_changed = true;
  }
  
/*____________________________________________________________________
|
| Return new position and heading
|___________________________________________________________________*/

  *new_position = current_position;
  *new_heading  = current_heading;
}
