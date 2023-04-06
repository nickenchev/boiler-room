#ifndef EDITORPART_H
#define EDITORPART_H

#include <vector>
#include <memory>
#include <optional>
#include "core/part.h"
#include "core/entity.h"
#include "display/glyphmap.h"
#include "assets/gltfimporter.h"
#include "assets/maps/sceneobject.h"

#define FILE_PATH_BUFF_SIZE 256

namespace Boiler
{
	class EntityComponentSystem;
    class GLTFModel;



class EditorPart : public Boiler::Part
{
	char modelFilePath[FILE_PATH_BUFF_SIZE];

    Boiler::GLTFImporter gltfImporter;

	bool windowModels, windowModelList, windowEntities, menuAddModel, menuNewEntity;

    std::vector<std::shared_ptr<Boiler::GLTFModel>> models;

	std::optional<int> objectIndex, modelIndex, meshIndex;
	std::vector<std::unique_ptr<Editor::SceneObject>> sceneObjects;
	
public:
	EditorPart(Boiler::Engine &engine);

    void onStart() override;
    void update(const Boiler::FrameInfo &frameInfo) override;
};

}
#endif /* EDITORPART_H */
