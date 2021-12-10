/// <title>Viewer GUI</title>
/// <desc>
///		Specific implementation of the ViewerInterface.
///		You could substitute a headless or some other graphics library implementation
///		in place of it and the rest of the application could operate without any issues.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <CImg/CImg.h>

#include "../ViewerInterface.h"
#include "../ControllerInterface.h"
#include "../ModelInterface.h"
#include "../model/PoseDataUtil.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/filebrowser/imfilebrowser.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ViewerGUI {

	/// <summary>
	///		Specific implementation of the ViewerInterface.
	///		You could substitute a headless or some other graphics library implementation
	///		in place of it and the rest of the application could operate without any issues.
	/// </summary>
	class ViewerGLFW : public PoseEditor::Viewer {
	private:
		/// <summary>GLFW application window</summary>
		GLFWwindow* m_Window;
		/// <summary>Controller to report any user input operations to:</summary>
		std::shared_ptr<PoseEditor::Controller> m_Controller;
		/// <summary>
		/// This is a copy of the model pawn updated by the controller whenever a change
		/// is propagated to the model.
		/// It is needed, because imgui constructs UI from scratch every update.
		/// The alternative would be reading from the model on every frame.
		/// </summary>
		PoseData::BonePawn m_InternalPawn;
		/// <summary> toggles display of indented hierarchy of bones. </summary>
		bool m_ShowHierarchy = false;
		/// <summary> toggles display of bone editing tools. </summary>
		bool m_ShowSimple = false;
		/// <summary> toggles window closing confirmation. Needed, because ImGui can only prompt popups during its update loop and closing may happen outside of that. </summary>
		bool m_PopupCloseNoSave = false;
		ImGui::FileBrowser m_FileOpenDialog;
		ImGui::FileBrowser m_FileSaveDialog;
		ImFont* m_Font;
		/// <summary>OpenGL texture bufffer handle for the up icon.</summary>
		int m_IndentCount = 0;
		/// <summary>OpenGL texture bufffer handle for the up icon.</summary>
		GLuint m_upIcon;
		/// <summary>OpenGL texture bufffer handle for the down icon.</summary>
		GLuint m_downIcon;
		/// <summary>OpenGL texture bufffer handle for the add child icon.</summary>
		GLuint m_childIcon;
		/// <summary>OpenGL texture bufffer handle for the remove bone icon.</summary>
		GLuint m_closeIcon;

		/// <summary>
		/// First step of the update loop. Calls upon ImGui to draw UI. (see RenderUI())
		/// </summary>
		void updateRender();

		/// <summary>
		/// Second step of the update loop. Handles GLFW window events.
		/// </summary>
		void updateEvents();

		/// <summary>
		/// Contains all the imgui draw calls in once place.
		/// </summary>
		void renderUI();

		/// <summary>
		/// Displays options related to a particular bone. (Within an existing imgui panel)
		/// </summary>
		/// <param name="boneidx">position of the drawn bone in the BonePawn bone array</param>
		/// <param name="bone">reference to the bone drawn</param>
		/// <param name="indent">amount of pixels to indent this line by. Used by hierarchy display.</param>
		void renderBoneUI(int boneidx, PoseData::BoneData& bone, int indent = 0);

		/// <summary>
		/// Displays indented hierarchy for all children of the bone with ID boneId.
		/// </summary>
		void recursiveHierarchyRenderUI(ID boneId);

		/// <summary>
		/// Loads the BMP image in the provided path into an OpenGL texture.
		/// Will not throw any exceptions since this application does not depend on this functionality and it is easy to trigger by moving the texture files.
		/// </summary>
		/// <returns>OpenGL texture id of the loaded texture</returns>
		GLuint createGLTextureBuffer(std::string path);
		/// <summary>
		/// To properly dispose of an OpenGL texture buffer.
		/// </summary>
		void destroyGLTextureBuffer(GLuint handle);

	public:
		/// <summary>
		/// Called before the update loop begins. Configures the UI and render pipeline.
		/// Returns false if the initialization fails.
		/// </summary>
		bool init() override;
		/// <summary>
		/// Updates the view logic during update loop.
		/// </summary>
		void update() override;
		/// <summary>
		/// Destroys this view and cleans up.
		/// </summary>
		void cleanUp() override;
		/// <summary>
		/// Use during initialization to pass pointer to the Controller component.
		/// This is needed, because this specific viewer also handles the input callbacks of the window.
		/// </summary>
		void setController(std::shared_ptr<PoseEditor::Controller> _controller) override;

		/// <summary>
		/// Called by the controller whenever the model changes.
		/// It is important that this reference is const to avoid improper application design.
		/// </summary>
		/// <param name="currentPawn">Const reference to the active pawn in the model.</param>
		void updateView(const PoseData::BonePawn currentPawn) override;
	};

}