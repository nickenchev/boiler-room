#ifndef GAMEPART_H
#define GAMEPART_H

#include "core/part.h"
#include "core/entity.h"
#include "display/glyphmap.h"

namespace Boiler
{
	class EntityComponentSystem;
}

class ScoringSystem;

struct TransformEditor
{
	std::array<float, 3> position, scale;
};

class GamePart : public Boiler::Part
{
	Boiler::Entity selectedEntity;
	bool windowEntities, menuNewEntity;

	TransformEditor transformEditor;
	
public:
	GamePart(Boiler::Engine &engine);

    void onStart() override;
    void update(const Boiler::FrameInfo &frameInfo) override;
};

#endif /* GAMEPART_H */
