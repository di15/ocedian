
 

class CPlayer
{
public:
	bool on;
	bool forward;
	bool backward;
	bool left;
	bool right;
	bool jump;

	CPlayer()
	{
		on = false;
		forward = false;
		backward = false;
		left = false;
		right = false;
		jump = false;
	}
};

#define PLAYERS		32
extern CPlayer g_player[PLAYERS];

extern int g_localP;