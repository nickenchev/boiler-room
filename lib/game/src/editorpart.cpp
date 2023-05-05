#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "core/entitycomponentsystem.h"
#include "display/skyboxloader.h"
#include "display/renderer.h"
#include "assets/maps/maploader.h"

#include "core/components/rendercomponent.h"
#include "physics/collidercomponent.h"
#include "physics/physicscomponent.h"
#include "core/componentmapper.h"
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

const char *defaultModelPath = "data/";
const char *PopupObjectTypeSelection = "object_type_selection";

using namespace Boiler;
using namespace Boiler::Editor;

std::array<ObjectType, 3> objectTypes{
	ObjectType::Mesh,
	ObjectType::Light,
	ObjectType::Camera
};

EditorPart::EditorPart(Engine &engine) : Part("Boiler Room", engine), gltfImporter(engine)
{
    windowModels = true;
	windowModelList = false;
	menuAddModel = false;
	windowEntities = true;
	menuNewEntity = false;

	strcpy(modelFilePath, "data/cabin.gltf");
	engine.setMouseRelativeMode(false);
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

	auto &camCollider = ecs.createComponent<ColliderComponent>(viewportCam);
	camCollider.colliderType = ColliderType::AABB;
	camCollider.isDynamic = true;
	camCollider.min = vec3(-0.5f, -0.5f, -0.5f);
	camCollider.min = vec3(0.5f, 0.5f, 0.5f);

	/*
	Entity gui = ecs.newEntity("gui");
    ecs.createComponent<GUIComponent>(gui, [this, &ecs] {
		if (ImGui::Begin("Scene", &windowModels, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::InputText("Model File Path", modelFilePath, FILE_PATH_BUFF_SIZE);
				if (ImGui::MenuItem("Add Model"))
				{
					models.push_back(gltfImporter.import(engine.getRenderer().getAssetSet(), modelFilePath));
				}
				ImGui::EndMenuBar();
			}

			ImGui::ShowDemoWindow();

			ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;
			// Display liste of loaded models
			if (ImGui::CollapsingHeader("Assets", headerFlags))
			{
				if (ImGui::BeginTable("Models", 2))
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Type");
					ImGui::TableHeadersRow();

					// iterate the loaded models
					for (int modelIdx = 0; modelIdx < models.size(); ++modelIdx)
					{
						auto &model = models[modelIdx];
						ImGui::TableNextRow();

						// file path
						ImGui::TableNextColumn();
						bool isOpen = ImGui::TreeNodeEx(model->getFilePath().c_str(), ImGuiTreeNodeFlags_SpanFullWidth);

						ImGui::TableNextColumn();
						ImGui::TextUnformatted("Model");

						if (isOpen)
						{
							// nodes
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							if (ImGui::TreeNodeEx("Nodes"))
							{
								ImGui::TableNextColumn();
								ImGui::TextDisabled("");

								if (model->getModel().nodes.size())
								{
									for (int nodeIdx = 0; nodeIdx < model->getModel().nodes.size(); ++nodeIdx)
									{
										const gltf::Node &node = model->getModel().nodes[nodeIdx];

										ImGui::TableNextColumn();

										bool isSelected = selectedNode == nodeIdx;
										if (ImGui::Selectable(node.name.c_str(), &isSelected))
										{
											selectedModel = modelIdx;
											selectedNode = nodeIdx;
										}
										if (ImGui::BeginPopupContextItem())
										{
											if (ImGui::Button("Generate Objects"))
											{
												logger.log("Generating objects");
												ImGui::CloseCurrentPopup();
											}
										}


										ImGui::TableNextColumn();
										ImGui::TextUnformatted("");
									}
								}
								ImGui::TreePop();
							}

							// meshes
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							if (ImGui::TreeNodeEx("Meshes"))
							{
								ImGui::TableNextColumn();
								ImGui::TextDisabled("");
								if (model->getModel().meshes.size())
								{
									ImGui::TableNextRow();
									for (int meshIdx = 0; meshIdx < model->getModel().meshes.size(); ++meshIdx)
									{
										const gltf::Mesh &mesh = model->getModel().meshes.at(meshIdx);
										ImGui::TableNextColumn();

										bool isSelected = selectedMesh == meshIdx;
										if (ImGui::Selectable(mesh.name.c_str(), &isSelected))
										{
											selectedModel = modelIdx;
											selectedMesh = meshIdx;
										}

										ImGui::TableNextColumn();
										ImGui::TextUnformatted(std::to_string(mesh.primitives.size()).c_str());
									}
								}
								ImGui::TreePop(); // mesh
							}
							ImGui::TreePop(); // model
						}
					}

					ImGui::EndTable();
				}
			}

			const int columnEntityId = 0;
			const int columnEntityName = 1;
			const int columnEntityDelete = 2;
			ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;

			if (ImGui::CollapsingHeader("Objects", headerFlags))
			{
				if (ImGui::Button("Add Object"))
				{
					ImGui::OpenPopup(PopupObjectTypeSelection);
				}
				if (ImGui::BeginTable("boiler_table_entities", 1, tableFlags, ImVec2(0, 300), 0))
				{
					ImGui::TableSetupColumn("Name");
					ImGui::TableHeadersRow();

					for (int i = 0; i < sceneObjects.size(); ++i)
					{
						const Entity &entity = sceneObjects[i]->getEntity();
						const std::string &name = sceneObjects[i]->getName();
						ImGui::PushID(entity.getId());
						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						bool isSelected = i == selectedObject;
						ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
						ImGui::Selectable(name.c_str(), &isSelected, selectableFlags, ImVec2(0, 0));

						if (isSelected)
						{
							selectedObject = i;
						}

						ImGui::PopID();
					}
					ImGui::EndTable();

				}
				if (ImGui::BeginPopup(PopupObjectTypeSelection))
				{
					for (auto objectType : objectTypes)
					{
						switch (objectType)
						{
							case ObjectType::Mesh:
							{
								if (ImGui::Selectable("Mesh"))
								{
									const std::string newName = fmt::format("Object {}", sceneObjects.size() + 1);
									Entity newEntity = ecs.newEntity(newName);
									sceneObjects.push_back(std::make_unique<SceneMesh>(newName, newEntity));

									ecs.createComponent<TransformComponent>(newEntity);
									ecs.createComponent<RenderComponent>(newEntity);
									selectedObject = sceneObjects.size() - 1;
								}
								break;
							}
							case ObjectType::Light:
							{
								if (ImGui::Selectable("Light"))
								{
									const std::string newName = fmt::format("Object {}", sceneObjects.size() + 1);
									Entity newEntity = ecs.newEntity(newName);
									sceneObjects.push_back(std::make_unique<SceneLight>(newName, newEntity));

									ecs.createComponent<TransformComponent>(newEntity);
									ecs.createComponent<LightingComponent>(newEntity, vec4(1, 1, 1, 1));
									selectedObject = sceneObjects.size() - 1;
								}
								break;
							}
							case ObjectType::Camera:
							{
								if (ImGui::Selectable("Camera"))
								{
									const std::string newName = fmt::format("Object {}", sceneObjects.size() + 1);
									Entity newEntity = ecs.newEntity(newName);
									sceneObjects.push_back(std::make_unique<SceneCamera>(newName, newEntity));

									ecs.createComponent<TransformComponent>(newEntity);
									selectedObject = sceneObjects.size() - 1;
								}
								break;
							}
						}

					}
					ImGui::EndPopup();
				}
			}
			if (selectedObject.has_value())
			{
				Entity selectedEntity = sceneObjects[selectedObject.value()]->getEntity();
				if (ImGui::CollapsingHeader("Components", headerFlags))
				{
					if (ecs.hasComponent<TransformComponent>(selectedEntity))
					{
						if (ImGui::TreeNode("Transform"))
						{
							TransformComponent &transform = ecs.retrieve<TransformComponent>(selectedEntity);

							unsigned short propertyId = 0;
							if (ImGui::BeginTable("Transform", 2))
							{
								ImGui::TableSetupColumn("Property");
								ImGui::TableSetupColumn("Value");
								ImGui::TableHeadersRow();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Translate X");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.translation.x);
								ImGui::PopID();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Translate Y");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.translation.y);
								ImGui::PopID();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Translate Z");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.translation.z);
								ImGui::PopID();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Scale X");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.scale.x);
								ImGui::PopID();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Scale Y");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.scale.y);
								ImGui::PopID();

								ImGui::PushID(propertyId++);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("Scale Z");
								ImGui::TableNextColumn();
								ImGui::InputFloat("", &transform.scale.z);
								ImGui::PopID();

								ImGui::EndTable();
							}

							ImGui::TreePop();
						}
					}
					else
					{
						if (ImGui::Button("Add Transform"))
						{
							ecs.createComponent<TransformComponent>(selectedEntity);
						}
					}

					if (ecs.hasComponent<RenderComponent>(selectedEntity))
					{
						auto &render = ecs.retrieve<RenderComponent>(selectedEntity);
						if (ImGui::TreeNode("Rendering"))
						{
							if (ImGui::Button("Set Mesh"))
							{
								if (selectedModel.has_value() && selectedMesh.has_value())
								{
									render.mesh = models[selectedModel.value()]->getImportResult().meshes[selectedMesh.value()];
								}
							}
							ImGui::TreePop();
						}
					}
					else
					{
						if (ImGui::Button("Add Render"))
						{
							ecs.createComponent<RenderComponent>(selectedEntity);
						}
					}
					if (ecs.hasComponent<ColliderComponent>(selectedEntity))
					{
						auto &collider = ecs.retrieve<ColliderComponent>(selectedEntity);
						if (ImGui::TreeNode("Collider"))
						{
							ImGui::TreePop();
						}
					}
					else
					{
						if (ImGui::Button("Add Collider"))
						{
							ecs.createComponent<ColliderComponent>(selectedEntity);
						}
					}
					if (ecs.hasComponent<LightingComponent>(selectedEntity))
					{
						auto &lighting = ecs.retrieve<LightingComponent>(selectedEntity);
						if (ImGui::TreeNode("Lighting"))
						{
							ImGui::TreePop();
						}
					}
				}
			}
		}
		ImGui::End();
	});
	*/
}

void EditorPart::update(const FrameInfo &frameInfo)
{
}
