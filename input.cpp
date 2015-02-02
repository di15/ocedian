

void Input()
{
	//float speed = (float)(CAMERA_SPEED);

	// Store the last position and view of the camera
	//CVector3 vOldPosition = Position();
	//CVector3 vOldView = View();

	// Use a flag to see if we movement backwards or not
	//bool bMovedBack = false;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here is where we subtract the gravity acceleration from our velocity vector.
	// We then add that velocity vector to our camera to effect our camera (or player)
	// This is also how we handle the jump velocity when we hit space bar.
	// Notice that we multiply the gravity by the frame interval (dt).  This makes
	// it so faster video cards don't do a 2 frame jump, while TNT2 cards do 20 frames :)
	// This is necessary to make every computer use the same movement and jump speed.
	//m_vVelocity.y -= (float)(GRAVITY * g_FrameInterval);
	//m_vPosition = m_vPosition + m_vVelocity;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	/*

	// Check if we hit the Up arrow or the 'w' key
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				

		// Move our camera forward by a positive SPEED
		Move(speed);				
	}

	// Check if we hit the Down arrow or the 's' key
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			

		// Move our camera backward by a negative SPEED
		Move(-speed);	
		//bMovedBack = true;
	}

	// Check if we hit the Left arrow or the 'a' key
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			

		// Strafe the camera left
		Strafe(-speed);
	}

	// Check if we hit the Right arrow or the 'd' key
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			

		// Strafe the camera right
		Strafe(speed);
	}	
	*/
	/*
	// Now that we moved, let's get the current position and test our movement
	// vector against the level data to see if there is a collision.
	CVector3 vCurrentPosition = Position();

	// Check for collision with AABB's and grab the new position
	CVector3 vNewPosition = g_map.TraceBox(vOldPosition, vCurrentPosition,
		                                     CVector3(-20, -50, -20), CVector3(20, 50, 20));

	// Check if we collided and we moved backwards
	if(g_map.Collided() && bMovedBack)
	{
		// If or x or y didn't move, then we are backed into a wall so restore the view vector
		if(vNewPosition.x == vOldPosition.x || vNewPosition.z == vOldPosition.z)
			m_vView = vOldView;		
	}

	// Set the new position that was returned from our trace function
	m_vPosition = vNewPosition;
	*/

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	/*
	// After we check for collision, we only want to add the velocity vector to
	// our view vector when we are falling.  If we aren't on the ground then
	// we don't want to push the the camera view down to the ground.  It's okay
	// if the position goes down because the collision detection fixes that so
	// we don't go through the ground, however, it's not natural to push the view
	// down too.  Well, assuming is strong enough to push our face down to the ground :)
	if(!g_map.IsOnGround())
		m_vView = m_vView + m_vVelocity;
	else
	{
		// If we ARE on the ground, we want to get rid of the jump acceleration
		// that we add when the user hits the space bar.  Below we check to see
		// if our velocity is below 0 then we are done with our jump and can just
		// float back to the ground by the gravity.  We do also add our gravity
		// acceleration to the velocity every frame, so this resets this to zero
		// for that as well.
		if(m_vVelocity.y < 0)
			m_vVelocity.y = 0;
	}*/

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}