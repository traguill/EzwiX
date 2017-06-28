#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define RELEASE( x )\
	{\
		if( x )\
		{\
			x->Release();\
			x = nullptr; \
		}\
	}

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

#endif // !__GLOBALS_H__
