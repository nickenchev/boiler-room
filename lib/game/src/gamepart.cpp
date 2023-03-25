#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <SDL_keycode.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "core/entitycomponentsystem.h"
#include "display/skyboxloader.h"
#include "display/renderer.h"
#include "assets/maps/maploader.h"

#include "camera/cameracomponent.h"
#include "physics/movementcomponent.h"
#include "input/inputcomponent.h"
#include "core/components/lightingcomponent.h"
#include "core/components/transformcomponent.h"
#include "physics/collidercomponent.h"
#include "physics/physicscomponent.h"
#include "core/components/textcomponent.h"
#include "assets/gltfimporter.h"
#include "core/glyphloader.h"

#include "util/colour.h"
#include "gamepart.h"

using namespace Boiler;

GamePart::GamePart(Engine &engine) : Part("Boiler Room", engine) 
{
	windowEntities = true;
	menuNewEntity = false;
	selectedEntity = Entity::NO_ENTITY;
}

void GamePart::onStart()
{
}

void GamePart::update(const FrameInfo &frameInfo)
{
}
