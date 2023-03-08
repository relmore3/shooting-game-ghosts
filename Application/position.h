/*____________________________________________________________________
|
| File: position.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

// move commands
#define POSITION_MOVE_FORWARD 0x1
#define POSITION_MOVE_BACK    0x2
#define POSITION_MOVE_RIGHT   0x4
#define POSITION_MOVE_LEFT    0x8

#define POSITION_ROTATE_UP    0x1
#define POSITION_ROTATE_DOWN  0x2
#define POSITION_ROTATE_RIGHT 0x4
#define POSITION_ROTATE_LEFT  0x8

#define RUN_SPEED (7.3f*3)  // feet per second (based on a 12-minute mile run)

// Init starting position, other parameters
void Position_Init (
  gx3dVector *position, 
  gx3dVector *heading,      // 0,0,1 for cubic environment mapping to work correctly (why?)
  float       move_speed ); // move speed in feet per second

// Free any resources
void Position_Free ();

// Sets new move speed (in fps)
void Position_Set_Speed (float move_speed);

// Update position
void Position_Update (
  unsigned    elapsed_time,
  unsigned    move,
  int         xrotate,
  int         yrotate,
  bool        update_all,       // boolean           
  bool       *position_changed, // returns true if position has changed, else false
  bool       *camera_changed,   // return true if heading has changed
  gx3dVector *new_position,
  gx3dVector *new_heading );
