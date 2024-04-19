	
	right_height=cp[vrb].sy-cp[vrt].sy;
	if (right_height==0)
		right_height=1;
		
	Delta_right_x=(cp[vrb].sx-cp[vrt].sx)/right_height;
	Right_x=cp[vrt].sx;

	Right_z=cp[vrt].z;
	Delta_right_z=(cp[vrb].z-cp[vrt].z)/right_height;

	next_break_right = cp[vrb].sy;
	
