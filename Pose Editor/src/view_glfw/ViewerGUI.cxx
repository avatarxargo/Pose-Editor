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

#define FOOTER_HEIGHT 30
#define INDENT_SIZE 20

#include "ViewerGUI.h"

bool ViewerGUI::ViewerGLFW::init() {

	/* configure glfw */
	if (!glfwInit())
		return false;

	m_Window = glfwCreateWindow(1280, 720, "Pose Editor", NULL, NULL);
	if (!m_Window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);
	glewInit();

	glClearColor(0.7f, 0.5f, 0.9f, 1.0f);

	/* configure imgui */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	/* Setup Platform/Renderer bindings */
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init(); //glsl_version

	/* configure file browser */
	m_FileOpenDialog = ImGui::FileBrowser();
	m_FileOpenDialog.SetTitle("open file");
	m_FileOpenDialog.SetTypeFilters({ ".csv" });

	m_FileSaveDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
	m_FileSaveDialog.SetTitle("save file");
	m_FileSaveDialog.SetTypeFilters({ ".csv" });

	/* load fonts */
	//m_Font = io.Fonts->AddFontFromFileTTF("data/arial.ttf", 10.0f);

	/* load textures */
	m_upIcon = createGLTextureBuffer("data/ico_up.bmp");
	m_downIcon = createGLTextureBuffer("data/ico_down.bmp");
	m_childIcon = createGLTextureBuffer("data/ico_child.bmp");
	m_closeIcon = createGLTextureBuffer("data/ico_close.bmp");

	return true;
}

void ViewerGUI::ViewerGLFW::renderUI() {
	/* top bar menu */
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "", false, true)) {
				m_Controller->cmdNewFile();
			}
			if (ImGui::MenuItem("Open", "", false, true)) {
				m_FileOpenDialog.SetPwd(m_FileOpenDialog.GetPwd()); //refresh folder
				m_FileOpenDialog.Open();
			}
			if (ImGui::MenuItem("Save", "", false, m_InternalPawn.loaded)) {
				m_Controller->cmdSaveFile(m_InternalPawn.originalFilePath);
			}
			if (ImGui::MenuItem("Save As")) {
				m_FileSaveDialog.SetPwd(m_FileSaveDialog.GetPwd()); //refresh folder
				m_FileSaveDialog.Open();
				m_FileSaveDialog.SetInputText(m_InternalPawn.originalFileName);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	/* file browsing dialogs */
	m_FileOpenDialog.Display();
	m_FileSaveDialog.Display();

	if (m_FileOpenDialog.HasSelected())
	{
		std::string openPath = m_FileOpenDialog.GetSelected().string();
		m_FileOpenDialog.ClearSelected();
		m_Controller->cmdOpenFile(openPath);
	}
	if (m_FileSaveDialog.HasSelected())
	{
		std::string savePath = m_FileSaveDialog.GetSelected().string();
		m_FileSaveDialog.ClearSelected();
		m_Controller->cmdSaveFile(savePath);
	}

	/* pop up messages */
	if (m_PopupCloseNoSave) {
		m_PopupCloseNoSave = false;
		ImGui::OpenPopup("confirmClose");
	}
	if (ImGui::BeginPopup("confirmClose")) {
		ImGui::Text("%s has unsaved changes. Are you sure you want to exit the application?", m_InternalPawn.originalFileName.c_str());
		if (ImGui::Button("Close Anyway")) {
			m_Controller->setApplicationActive(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	/* main editor window */
	ImGui::Begin("Bone Editor"); {
		/* File Info */
		float maxw = ImGui::GetContentRegionAvail().x;
		ImGui::Text("%s", m_InternalPawn.originalFileName.c_str());
		/* simple toggle */
		ImGui::SameLine();
		ImGui::SetCursorPosX(maxw - 170);
		ImGui::Text("simple");
		ImGui::SameLine();
		ImGui::SetCursorPosX(maxw - 120);
		ImGui::PushID("simple");
		ImGui::Checkbox("", &m_ShowSimple);
		ImGui::PopID();
		/* hierarchy toggle */
		ImGui::SameLine();
		ImGui::SetCursorPosX(maxw - 85);
		ImGui::Text("hierarchy");
		ImGui::SameLine();
		ImGui::SetCursorPosX(maxw - 15);
		ImGui::PushID("hierarchy");
		ImGui::Checkbox("", &m_ShowHierarchy);
		ImGui::PopID();
		ImGui::Separator();

		/* Node Inspectors */
		ImGui::BeginChild("node block", ImVec2(-1, (-FOOTER_HEIGHT - 5 + ImGui::GetContentRegionAvail().y))); {
			if (m_ShowHierarchy) {
				/* traverse from root elements with indentation */
				auto roots = PoseDataUtil::pawnGetBoneChildren(m_InternalPawn, -1);
				for (auto rootidx : roots)
				{
					auto& bone = m_InternalPawn.bones[rootidx];
					renderBoneUI(rootidx, bone);
					m_IndentCount = 0;
					recursiveHierarchyRenderUI(bone.id);
				}
			}
			else {
				/* traverse regularly */
				int boneidx = 0;
				for (PoseData::BoneData bone : m_InternalPawn.bones) {
					renderBoneUI(boneidx, bone);
					boneidx++;
				}
			}
		}
		ImGui::EndChild();

		/* footer with a bone spawning button */
		ImGui::BeginChild("footer block", ImVec2(-1, FOOTER_HEIGHT));
		ImGui::Separator();
		if (ImGui::Button("+ bone")) {
			m_Controller->cmdBoneAdd(-1);
		}
		ImGui::EndChild();

	}
	ImGui::End();
}

void ViewerGUI::ViewerGLFW::renderBoneUI(int boneidx, PoseData::BoneData& bone, int indent) {
	ImGui::PushID(boneidx);
	float indentDistance = static_cast<float>(indent) * INDENT_SIZE;
	float maxw = ImGui::GetContentRegionAvail().x;
	ImGui::TextColored(ImVec4(0.71f, 0.30f, 0.62f, 1.f), ("[" + std::to_string(bone.id) + "] " + bone.displayName).c_str());

	/* controls */
	if (!m_ShowHierarchy) {
		ImGui::SameLine();
		ImGui::PushID("up");
		ImGui::SetCursorPosX(indentDistance + maxw - 100);
		if (ImGui::ImageButton((void*)m_upIcon, { 15,15 })) {
			m_Controller->cmdBoneMoveUp(bone.id);
		}
		ImGui::PopID();

		ImGui::SameLine();
		ImGui::PushID("down");
		ImGui::SetCursorPosX(indentDistance + maxw - 75);
		if (ImGui::ImageButton((void*)m_downIcon, { 15,15 })) {
			m_Controller->cmdBoneMoveDown(bone.id);
		}
		ImGui::PopID();
	}

	ImGui::SameLine();
	ImGui::PushID("addchild");
	ImGui::SetCursorPosX(indentDistance + maxw - 50);
	if (ImGui::ImageButton((void*)m_childIcon, { 15,15 })) {
		m_Controller->cmdBoneAdd(bone.id);
	}
	ImGui::PopID();

	ImGui::SameLine();
	ImGui::PushID("remove");
	ImGui::SetCursorPosX(indentDistance + maxw - 25);
	if (ImGui::ImageButton((void*)m_closeIcon, { 15,15 })) {
		m_Controller->cmdBoneRemove(bone.id);
	}
	ImGui::PopID();

	/* only show editor when simple is unchecked */
	if (!m_ShowSimple) {
		ImGui::PushItemWidth(-1);

		/* name */
		ImGui::Text("name");
		ImGui::SameLine();
		ImGui::SetCursorPosX(indentDistance + 50);
		if (ImGui::InputText("", &(bone.displayName))) {
			m_Controller->cmdBoneSetName(bone.id, bone.displayName);
		}

		/* parent */
		ImGui::Text("parent");
		ImGui::SameLine();
		ImGui::SetCursorPosX(indentDistance + 50);
		std::string selectedName = "[Root]";
		int pcoord = PoseDataUtil::pawnFindBoneId(m_InternalPawn, bone.parent);
		if (pcoord >= 0) {
			selectedName = m_InternalPawn.bones[pcoord].displayName;
		}
		if (ImGui::BeginCombo("combo", selectedName.c_str())) {
			if (ImGui::Selectable("[Root]", pcoord < 0)) {
				m_Controller->cmdBoneSetParent(bone.id, -1);
			}
			int pidx = 0;
			for (PoseData::BoneData pbone : m_InternalPawn.bones) {
				if (ImGui::Selectable(pbone.displayName.c_str(), pidx++ == pcoord)) {
					m_Controller->cmdBoneSetParent(bone.id, pbone.id);
				}
			}
			ImGui::EndCombo();
		}

		/* angle */
		ImGui::Text("angle");
		ImGui::SameLine();
		ImGui::SetCursorPosX(indentDistance + 50);
		ImGui::PopItemWidth();
		ImGui::PushItemWidth((maxw - 50) / 3 - 10);
		ImGui::PushID("angx");
		if (ImGui::SliderFloat("", &glm::value_ptr(bone.eulerRotation)[0], -179.f, 179.f)) {
			m_Controller->cmdBoneSetRotation(bone.id, bone.eulerRotation);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID("angy");
		ImGui::SetCursorPosX(indentDistance + 50 + (maxw - 50) / 3);
		if (ImGui::SliderFloat("", &glm::value_ptr(bone.eulerRotation)[1], -89.f, 89.f)) {
			m_Controller->cmdBoneSetRotation(bone.id, bone.eulerRotation);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID("angz");
		ImGui::SetCursorPosX(indentDistance + 50 + 2 * (maxw - 50) / 3);
		if (ImGui::SliderFloat("", &glm::value_ptr(bone.eulerRotation)[2], -179.f, 179.f)) {
			m_Controller->cmdBoneSetRotation(bone.id, bone.eulerRotation);
		}
		ImGui::PopID();

		ImGui::PopItemWidth();
	}
	ImGui::Separator();
	ImGui::PopID();
}

void ViewerGUI::ViewerGLFW::recursiveHierarchyRenderUI(ID boneId) {
	auto children = PoseDataUtil::pawnGetBoneChildren(m_InternalPawn, boneId);
	ImGui::Indent(INDENT_SIZE);
	m_IndentCount++;
	for (auto childidx : children)
	{
		auto& bone = m_InternalPawn.bones[childidx];
		renderBoneUI(childidx, bone, m_IndentCount);
		recursiveHierarchyRenderUI(bone.id);
	}
	ImGui::Unindent(INDENT_SIZE);
	m_IndentCount--;
}

GLuint ViewerGUI::ViewerGLFW::createGLTextureBuffer(std::string path) {
	GLuint tex = 0;

	try {
		cimg_library::CImg<unsigned char> imgdata(path.c_str());
		imgdata.permute_axes("cxyz");
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, imgdata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch (const std::exception&)
	{
		//these icons are just for visual flare, so this catch prevents the application from failing to a missing texture. No need to handle further.
	}
	return tex;
}

void ViewerGUI::ViewerGLFW::destroyGLTextureBuffer(GLuint handle) {
	glDeleteBuffers(1, &handle);
}

void ViewerGUI::ViewerGLFW::updateRender() {
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	renderUI();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	int display_w, display_h;
	glfwGetFramebufferSize(m_Window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glfwSwapBuffers(m_Window);
}

void ViewerGUI::ViewerGLFW::updateEvents() {
	glfwPollEvents();

	/* Detect closing of the window */
	if (glfwWindowShouldClose(m_Window)) {
		if (!m_InternalPawn.saved) {
			m_PopupCloseNoSave = true;
			glfwSetWindowShouldClose(m_Window, false);
		}
		else {
			m_Controller->setApplicationActive(false);
		}
	}
}

void ViewerGUI::ViewerGLFW::update() {
	updateRender();
	updateEvents();
}

void ViewerGUI::ViewerGLFW::cleanUp() {
	/* clean up imgui */
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	/* unload textures */
	destroyGLTextureBuffer(m_upIcon);
	destroyGLTextureBuffer(m_downIcon);
	destroyGLTextureBuffer(m_childIcon);
	destroyGLTextureBuffer(m_closeIcon);

	// This will kill glfw for all other Application Instances on this thread,
	// but if they are using another kind of viewer they will be fine.
	// It is a little inelegant, but I prefer this over polluting non-viewer components with glfw references.
	glfwTerminate();
}

void ViewerGUI::ViewerGLFW::updateView(const PoseData::BonePawn currentPawn) {
	m_InternalPawn = PoseDataUtil::pawnDeepCopy(currentPawn);

	std::stringstream ss("");
	if (m_InternalPawn.loaded)
		ss << "Pose Editor - " << m_InternalPawn.originalFilePath;
	else
		ss << "Pose Editor";
	if (!m_InternalPawn.saved)
		ss << "* - Unsaved Changes";
	ss.flush();
	glfwSetWindowTitle(m_Window, ss.str().c_str());
	// the UI will adapt on the next render call.
}

void ViewerGUI::ViewerGLFW::setController(std::shared_ptr<PoseEditor::Controller> _controller) { m_Controller = _controller; }