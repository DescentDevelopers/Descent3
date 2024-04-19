	left_height=cp[vlb].sy-cp[vlt].sy;
	if (left_height==0)
		left_height=1;


	Left_x=cp[vlt].sx;
	Left_z=cp[vlt].z;
	Delta_left_z=(cp[vlb].z-cp[vlt].z)/left_height;
	Delta_left_x=(cp[vlb].sx-cp[vlt].sx)/left_height;
	next_break_left = cp[vlb].sy;
	
