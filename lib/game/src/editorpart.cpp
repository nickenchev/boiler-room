#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

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
#include "core/glyphloader.h"
#include "core/components/guicomponent.h"

#include "util/colour.h"
#include "editorpart.h"

#include "imgui/imgui.h"

using namespace Boiler;

EditorPart::EditorPart(Engine &engine) : Part("Boiler Room", engine), gltfImporter(engine)
{
    windowModels = true;
	windowModelList = false;
	menuAddModel = false;
	windowEntities = true;
	menuNewEntity = false;
	selectedEntity = Entity::NO_ENTITY;
}

void EditorPart::onStart()
{
	auto& ecs = engine.getEcs();

	// create viewport camera

	Entity viewportCam = ecs.newEntity("Viewport Camera");
	ecs.createComponent<InputComponent>(viewportCam);
	ecs.createComponent<TransformComponent>(viewportCam);
	auto &camComponent = ecs.createComponent<CameraComponent>(viewportCam);
	camComponent.direction = vec3(0, 0, -1);
	camComponent.up = vec3(0, 1, 0);
	ecs.createComponent<MovementComponent>(viewportCam);
	auto &camPhysics = ecs.createComponent<PhysicsComponent>(viewportCam);
	camPhysics.speed = 0.1f;
	camPhysics.acceleration = 0.25f;

	Entity gui = ecs.newEntity("gui");
    ecs.createComponent<GUIComponent>(gui, [this, &ecs] {
		if (ImGui::Begin("Assets", &windowModels, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::MenuItem("Add Model"))
                {
					models.push_back(gltfImporter.import(engine.getRenderer().getAssetSet(), "data/terrain/terrain.gltf"));
                }
				ImGui::EndMenuBar();
			}

			// Display liste of loaded models
			if (ImGui::BeginTable("ModelAssets", 2, 0))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_None);
				ImGui::TableHeadersRow();

                for (auto model : models)
                {
					// File path
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
					ImGui::Text(model->getFilePath().c_str());

					// Actions
                    ImGui::TableSetColumnIndex(1);

					if (ImGui::Button("Instantiate"))
					{
						Entity newEntity = ecs.newEntity("New Entity");
						model->createInstance(ecs, newEntity);
						ecs.createComponent<TransformComponent>(newEntity);
					}
                }

				ImGui::EndTable();
			}

			ImGui::End();
		}

		ImGui::Begin("Entities", &windowEntities, ImGuiWindowFlags_MenuBar);
		{

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Entity", "CTRL+N"))
					{
						ecs.newEntity("New Entity");
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			static ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
				| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
				| ImGuiTableFlags_SizingFixedFit;

			const int columnEntityId = 0;
			const int columnEntityName = 1;
			const int columnEntityDelete = 2;

			if (ImGui::BeginTable("boiler_table_entities", 3, 0, ImVec2(0, 0), 0))
			{
				ImGui::TableSetupColumn("ID");
				ImGui::TableSetupColumn("Name");
				ImGui::TableHeadersRow();

				unsigned int i = 0;
				for (const Entity &entity : ecs.getEntityWorld().getEntities())
				{
					std::string label = fmt::format("{}", entity.getId());
                    ImGui::PushID(entity.getId());
                    ImGui::TableNextRow();

					ImGui::TableNextColumn();
					bool isEntitySelected = false;
					ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                    ImGui::Selectable(label.c_str(), &isEntitySelected, selectableFlags, ImVec2(0, 0));

					ImGui::TableNextColumn();
					ImGui::TextUnformatted(ecs.nameOf(entity).c_str());

					if (isEntitySelected)
					{
						selectedEntity = entity;

						auto &transform = ecs.retrieve<TransformComponent>(selectedEntity);
						vec3 entPos = transform.getPosition();
						vec3 entScale = transform.getScale();

						transformEditor.position[0] = entPos.x;
						transformEditor.position[1] = entPos.y;
						transformEditor.position[2] = entPos.z;

						transformEditor.scale[0] = entScale.x;
						transformEditor.scale[1] = entScale.y;
						transformEditor.scale[2] = entScale.z;
					}

					ImGui::PopID();
					++i;
				}
				ImGui::EndTable();

				if (selectedEntity != Entity::NO_ENTITY)
				{
					ImGui::Begin("Components", &windowEntities, ImGuiWindowFlags_MenuBar);
					{
						if (ecs.hasComponent<TransformComponent>(selectedEntity))
						{
							if (ImGui::CollapsingHeader("Transform"))
							{
								auto &transform = ecs.retrieve<TransformComponent>(selectedEntity);
								if (ImGui::InputFloat3("position", transformEditor.position.data()))
								{
									transform.setPosition(vec3(transformEditor.position[0], transformEditor.position[1], transformEditor.position[2]));
								}
								if (ImGui::InputFloat3("scale", transformEditor.scale.data()))
								{
									transform.setScale(vec3(transformEditor.scale[0], transformEditor.scale[1], transformEditor.scale[2]));
								}
							}
						}
					}
					ImGui::End();
				}
			}
		}
		ImGui::End();

	});
}

void EditorPart::update(const FrameInfo &frameInfo)
{
}
