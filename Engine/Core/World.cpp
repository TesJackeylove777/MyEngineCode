#include "World.h"
#include "Camera.h"

CWorld::CWorld()
{
	Camera = CreatObject<CCamera>(new CCamera());
}
