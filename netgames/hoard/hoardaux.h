#ifndef __HOARD_AUX_H_
#define __HOARD_AUX_H_

typedef struct{
	int min_hoard;
}tGameConfig;

//game config send/receive
void SendGameConfig(int towho);


#endif