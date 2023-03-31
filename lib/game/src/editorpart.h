#ifndef EDITORPART_H
#define EDITORPART_H

#include <vector>
#include <memory>
#include "core/part.h"
#include "core/entity.h"
#include "display/glyphmap.h"
#include "assets/gltfimporter.h"

namespace Boiler
{
	class EntityComponentSystem;
    class GLTFModel;
}

struct TransformEditor
{
	std::array<float, 3> position, scale;
};

class EditorPart : public Boiler::Part
{
    Boiler::GLTFImporter gltfImporter;

    Boiler::Entity selectedEntity;
	bool windowModels, windowModelList, windowEntities, menuAddModel, menuNewEntity;

    std::vector<std::shared_ptr<Boiler::GLTFModel>> models;

	TransformEditor transformEditor;
	
public:
	EditorPart(Boiler::Engine &engine);

    void onStart() override;
    void update(const Boiler::FrameInfo &frameInfo) override;
};

#endif /* EDITORPART_H */
