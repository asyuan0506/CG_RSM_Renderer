#include "ui_manager.h"

extern void ReleaseResources();
extern void LoadObjects(const std::string&);
extern void CreateLights();
extern void CreateCamera();
extern void CreateSkybox(const std::string texFilePath, int numSlices = 36, int numStacks = 18, float radius = 50.0f);
extern void CreateShaderLib();

std::string OpenFileDialog(LPCSTR filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

UIManager* UIManager::instance_ = nullptr;

UIManager::UIManager() {
    instance_ = this;
    
	render_window_ = -1;
    // Transformation
	scale_live_var_ = 1.5f;
    translate_xy_live_var_[0] = 0.0f; // Translation values for XY plane.
	translate_xy_live_var_[1] = 0.0f; // Translation values for XY plane.
	translate_z_live_var_[0] = 0.0f;  // Translation value for Z axis.
	point_light_translate_xy_live_var_[0] = 0.0f; // Light source translation values for XY plane.
	point_light_translate_xy_live_var_[1] = 0.0f; // Light source translation values for XY plane.
	point_light_translate_z_live_var_[0] = 0.0f;  // Light source translation value for Z axis.
	spot_light_translate_xy_live_var_[0] = 0.0f; // Spot Light source translation values for XY plane.
	spot_light_translate_xy_live_var_[1] = 0.0f; // Spot Light source translation values for XY plane.
	spot_light_translate_z_live_var_[0] = 0.0f;  // Spot Light source translation value for Z axis.
    spot_light_dir_x_live_var_ = 0.0f;
	spot_light_dir_y_live_var_ = 0.0f;
	spot_light_dir_z_live_var_ = 0.0f;
    for (int i = 0; i < 16; i++) {    // 4x4 rotation matrix (Column-major order)
        if (i % 5 == 0)
            rotate_matrix_live_var_[i] = 1.0f;
        else
			rotate_matrix_live_var_[i] = 0.0f;
    } 

	model_list_ = nullptr; 
	selected_model_ = nullptr;
	selected_point_light_ = nullptr;
	skybox_ = nullptr;
}

void UIManager::ChooseModelCB(int state) {
    if (state == 0) {
        int model_id = model_list_box_->get_int_val();
        SetSelectedModel((*model_list_)[model_id]);
        if (selected_model_ == nullptr) return;
        scale_live_var_ = selected_model_->GetScaleVector()[0]; // Uniform scale
        glm::vec3 translate_vec = selected_model_->GetTranslateVector();
        translate_xy_live_var_[0] = translate_vec[0];
        translate_xy_live_var_[1] = translate_vec[1];
        translate_z_live_var_[0] = translate_vec[2];

        glm::mat4x4 rotate_mat = selected_model_->GetRotateMatrix();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                rotate_matrix_live_var_[4 * i + j] = rotate_mat[i][j];

        glui_->sync_live(); // Update user interface to reflect changes

		//glutPostRedisplay();
    }
    else if (state == 1) {
        int model_id = model_list_box_->get_int_val();
        AddOrDeleteModel(true, model_id); // Delete model from list box
	}
	else if (state == 2) { // Rotate skybox checkbox changed
        if (rotate_skybox_checkbox_->get_int_val() == 1) { // Rotate skybox
            glm::mat4x4 rotate_mat = skybox_->GetRotateMatrix();
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    rotate_matrix_live_var_[4 * i + j] = rotate_mat[i][j];
			glui_->sync_live(); // Update user interface to reflect changes
		}
        else {
			ChooseModelCB(0); // Select the currently selected model again to update UI
        }
    }
    else if (state == 3) { // Use skybox checkbox changed
        glutSetWindow(render_window_);
        glutPostRedisplay();
	}
    else if (state == 4) { // Point Light selection changed
        int light_id = point_light_list_box_->get_int_val();
        SetSelectedPointLight((*point_light_list_)[light_id]);
        if (selected_point_light_ == nullptr) return;
        glm::vec3 light_pos = selected_point_light_->GetPosition();
        point_light_translate_xy_live_var_[0] = light_pos[0];
        point_light_translate_xy_live_var_[1] = light_pos[1];
        point_light_translate_z_live_var_[0] = light_pos[2];
		glui_->sync_live(); // Update user interface to reflect changes
    }
    else if (state == 5) { // Spot Light selection changed
        int light_id = spot_light_list_box_->get_int_val();
        SetSelectedSpotLight((*spot_light_list_)[light_id]);
        if (selected_spot_light_ == nullptr) return;
        glm::vec3 light_pos = selected_spot_light_->GetPosition();
        spot_light_translate_xy_live_var_[0] = light_pos[0];
        spot_light_translate_xy_live_var_[1] = light_pos[1];
        spot_light_translate_z_live_var_[0] = light_pos[2];
        glm::vec3 light_dir = selected_spot_light_->GetDirection();
        spot_light_dir_x_live_var_ = light_dir[0];
        spot_light_dir_y_live_var_ = light_dir[1];
        spot_light_dir_z_live_var_ = light_dir[2];
        glui_->sync_live(); // Update user interface to reflect changes
    }
}

void UIManager::AddOrDeleteModel(bool deletion, const int& id, std::string file_path) {
	// Handle model addition or deletion
	if (deletion) { // TODO: Select another model after deletion, protect against deleting when no model exists
        model_list_box_->delete_item(id);
        delete (*model_list_)[id];
        (*model_list_)[id] = nullptr;
        glutPostRedisplay();
    }
    else {
        std::string name = file_path.substr(file_path.find_last_of("\\") + 1);
        name.pop_back(); name.pop_back(); name.pop_back(); name.pop_back(); // remove ".obj"
		name += " " + std::to_string(id);
        model_list_box_->add_item(id, name);
		model_list_box_->set_int_val(id);
		ChooseModelCB(0); // Select the newly added model
    }
}

void UIManager::AddOrDeleteLight(bool deletion, const int& id, const int& type) {
    if (deletion) {
        if (type == 0) { // Point Light
            point_light_list_box_->delete_item(id);
            delete (*point_light_list_)[id];
            (*point_light_list_)[id] = nullptr;
        }
        else if (type == 1) { // Spot Light
            spot_light_list_box_->delete_item(id);
            delete (*spot_light_list_)[id];
            (*spot_light_list_)[id] = nullptr;
        }
        glutPostRedisplay();
    }
    else {
        if (type == 0) { // Point Light
            std::string name = "Point Light " + std::to_string(id);
            point_light_list_box_->add_item(id, name);
            point_light_list_box_->set_int_val(id);
            ChooseModelCB(4); // Select the newly added light
        }
        else if (type == 1) { // Spot Light
            std::string name = "Spot Light " + std::to_string(id);
            spot_light_list_box_->add_item(id, name);
            spot_light_list_box_->set_int_val(id);
            ChooseModelCB(5); // Select the newly added light
        }
	}
}

void UIManager::LoadModelButtonCB(int state) {
    if (state == 0) {
        std::string filePath = OpenFileDialog("OBJ Files\0*.obj\0All Files\0*.*\0");
        if (!filePath.empty()) {
            glutSetWindow(render_window_);

            LoadObjects(filePath);
            
            glutPostRedisplay();
        }
    }
}

void UIManager::LoadSkyboxButtonCB(int state) {
    if (state == 2) {
        std::string tex_file_path = OpenFileDialog("Image Files(*.JPG;*.PNG)\0*.JPG;*.PNG\0All files (*.*)\0*.*");
        if (!tex_file_path.empty()) {
            glutSetWindow(render_window_);
            ResetSceneButtonCB(2); // Reset Skybox
            CreateSkybox(tex_file_path);
            glutPostRedisplay();
        }
    }
}

void UIManager::RenderModeButtonCB(int state) {
    glutSetWindow(render_window_);
	int mode = render_mode_radio_group_->get_int_val();
    switch (mode) {
    case 0: // Point Mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case 1: // Line Mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 2: // Fill Mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void UIManager::ResetScene() {
    scale_live_var_ = 1.5f;
    translate_xy_live_var_[0] = 0.0f;
    translate_xy_live_var_[1] = 0.0f;
    translate_z_live_var_[0] = 0.0f;
    for (int i = 0; i < 16; i++)
        rotate_matrix_live_var_[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    for (int i = 0; i < model_list_->size(); i++) {
        if ((*model_list_)[i]) {
			AddOrDeleteModel(true, i);
        }
	}
	selected_model_ = nullptr;
	SetSelectedPointLight((*point_light_list_)[0]); // Select first light(Point Light)

    render_mode_radio_group_->set_int_val(2); // Set UI to Fill Mode
	point_light_list_box_->set_int_val(0); // Set UI to first light(Point Light)
	spot_light_list_box_->set_int_val(0); // Set UI to first light(Spot Light)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void UIManager::ResetSceneButtonCB(int state) {
    if (state == 0) {
        glutSetWindow(render_window_);
        ResetScene();
        glui_->sync_live(); // Update user interface to reflect changes
        glutPostRedisplay();
        return;
    }
    if (state == 2) { // Skybox reset
        glutSetWindow(render_window_);
		glm::mat4x4 identity_mat(1.0f);
		skybox_->SetRotateMatrix(identity_mat);
        if (rotate_skybox_checkbox_->get_int_val()) {
            for (int i = 0; i < 16; i++)
                rotate_matrix_live_var_[i] = (i % 5 == 0) ? 1.0f : 0.0f;
            glui_->sync_live(); // Update user interface to reflect changes
		}
        glutPostRedisplay();
        return;
	}
}

void UIManager::ModelTransformCB(int state) {
    if (state == 1 && rotate_skybox_checkbox_->get_int_val()) // Rotate skybox
    {
        skybox_->SetRotateMatrix(glm::make_mat4x4(rotate_matrix_live_var_));
        return;
	}
    if (state == 1 && selected_model_ != nullptr) {
        selected_model_->SetScaleVector(glm::vec3(scale_live_var_));
        selected_model_->SetRotateMatrix(glm::make_mat4x4(rotate_matrix_live_var_));
        selected_model_->SetTranslateVector(glm::vec3(translate_xy_live_var_[0], translate_xy_live_var_[1], translate_z_live_var_[0]));
		selected_model_->UpdateWorldMatrix();
    }
    if (state == 4 && selected_point_light_ != nullptr) { // Light translation changed
		selected_point_light_->SetPosition(glm::vec3(point_light_translate_xy_live_var_[0], 
                                                    point_light_translate_xy_live_var_[1], 
                                                    point_light_translate_z_live_var_[0]));
	}
    if (state == 5 && selected_spot_light_ != nullptr) { // Spot Light translation changed
        selected_spot_light_->SetPosition(glm::vec3(spot_light_translate_xy_live_var_[0],
                                                    spot_light_translate_xy_live_var_[1],
                                                    spot_light_translate_z_live_var_[0]));
        selected_spot_light_->SetDirection(glm::vec3(spot_light_dir_x_live_var_,
                                                     spot_light_dir_y_live_var_,
                                                     spot_light_dir_z_live_var_));
	}
}

// Static callback wrappers
void UIManager::ChooseModelCB_Static(int state) {
    if (instance_) instance_->ChooseModelCB(state);
}
void UIManager::LoadModelButtonCB_Static(int state) {
    if (instance_) instance_->LoadModelButtonCB(state);
}
void UIManager::LoadSkyboxButtonCB_Static(int state) {
    if (instance_) instance_->LoadSkyboxButtonCB(state);
}
void UIManager::RenderModeButtonCB_Static(int mode) {
    if (instance_) instance_->RenderModeButtonCB(mode);
}
void UIManager::ModelTransformCB_Static(int state) {
    if (instance_) instance_->ModelTransformCB(state);
}
void UIManager::ResetSceneButtonCB_Static(int state) {
    if (instance_) instance_->ResetSceneButtonCB(state);
}
void UIManager::LightOnOffButtonCB_Static(int state) {
    if (instance_) instance_->show_light_ = !instance_->show_light_;
	glutPostRedisplay();
}

void UIManager::InitGLUI(const int& render_window, const int& screenWidth) {
    glui_ = GLUI_Master.create_glui("Control Panel", 0, screenWidth + 100, 100);

	glui_->add_statictext("Render Competition");

    GLUI_Panel* model_panel = glui_->add_panel("Model");
    model_list_box_ = glui_->add_listbox_to_panel(model_panel, "Choose Model", nullptr, 0, ChooseModelCB_Static);
    glui_->add_separator_to_panel(model_panel);
    glui_->add_button_to_panel(model_panel, "Load Model", 0, LoadModelButtonCB_Static);
    glui_->add_button_to_panel(model_panel, "Delete Model", 1, ChooseModelCB_Static);
    glui_->add_button_to_panel(model_panel, "Reset", 0, ResetSceneButtonCB_Static);

    GLUI_Panel* skybox_panel = glui_->add_panel("Skybox");
	// TODO: Add 3 parameters for skybox creation
	use_skybox_checkbox_ = glui_->add_checkbox_to_panel(skybox_panel, "Use Skybox", nullptr, 3, ChooseModelCB_Static);
    glui_->add_button_to_panel(skybox_panel, "Load Skybox", 2, LoadSkyboxButtonCB_Static);
    glui_->add_button_to_panel(skybox_panel, "Reset", 2, ResetSceneButtonCB_Static);

	GLUI_Panel* render_mode_panel = glui_->add_panel("Render Mode");
    render_mode_radio_group_ = glui_->add_radiogroup_to_panel(render_mode_panel, nullptr, 0, RenderModeButtonCB_Static);
	render_mode_radio_group_->set_int_val(2); // Default to Fill Mode
    glui_->add_radiobutton_to_group(render_mode_radio_group_, "Point Mode");
    glui_->add_radiobutton_to_group(render_mode_radio_group_, "Line Mode");
    glui_->add_radiobutton_to_group(render_mode_radio_group_, "Fill Mode");
    
    glui_->add_column(true);

    GLUI_Panel* transform_panel = glui_->add_panel("Transform Controls");
    scale_spinner_ = glui_->add_spinner_to_panel(transform_panel, "Scale", GLUI_SPINNER_FLOAT, &scale_live_var_, 1, ModelTransformCB_Static);
    scale_spinner_->set_float_limits(0.1f, 50.0f); scale_spinner_->set_speed(0.005f);
	glui_->add_separator_to_panel(transform_panel);
    xy_translator_ = glui_->add_translation_to_panel(transform_panel, "Translation XY", GLUI_TRANSLATION_XY, translate_xy_live_var_, 1, ModelTransformCB_Static);
    xy_translator_->set_speed(0.01f);
    z_translator_ = glui_->add_translation_to_panel(transform_panel, "Translation Z", GLUI_TRANSLATION_Z, translate_z_live_var_, 1, ModelTransformCB_Static);
    z_translator_->set_speed(0.01f);
    glui_->add_separator_to_panel(transform_panel);
	rotate_skybox_checkbox_ = glui_->add_checkbox_to_panel(transform_panel, "Rotate Skybox", nullptr, 2, ChooseModelCB_Static);
    rotator_ = glui_->add_rotation_to_panel(transform_panel, "Rotation", rotate_matrix_live_var_, 1, ModelTransformCB_Static);

    glui_->add_column(true);

	GLUI_Panel* light_panel = glui_->add_panel("Lights");
    glui_->add_button_to_panel(light_panel, "On/Off", 4, LightOnOffButtonCB_Static);
	// Point Light Controls
    point_light_list_box_ = glui_->add_listbox_to_panel(light_panel, "Point Light", nullptr, 4, ChooseModelCB_Static);
    point_light_xy_translator_ = glui_->add_translation_to_panel(light_panel, "Translation XY", GLUI_TRANSLATION_XY, point_light_translate_xy_live_var_, 4, ModelTransformCB_Static);
    point_light_xy_translator_->set_speed(0.01f);
    point_light_z_translator_ = glui_->add_translation_to_panel(light_panel, "Translation Z", GLUI_TRANSLATION_Z, point_light_translate_z_live_var_, 4, ModelTransformCB_Static);
    point_light_z_translator_->set_speed(0.01f);
    glui_->add_separator_to_panel(light_panel);
	// Spot Light Controls
    spot_light_list_box_ = glui_->add_listbox_to_panel(light_panel, "Spot Light", nullptr, 5, ChooseModelCB_Static);
    spot_light_xy_translator_ = glui_->add_translation_to_panel(light_panel, "Translation XY", GLUI_TRANSLATION_XY, spot_light_translate_xy_live_var_, 5, ModelTransformCB_Static);
	spot_light_xy_translator_->set_speed(0.01f);
    spot_light_z_translator_ = glui_->add_translation_to_panel(light_panel, "Translation Z", GLUI_TRANSLATION_Z, spot_light_translate_z_live_var_, 5, ModelTransformCB_Static);
	spot_light_z_translator_->set_speed(0.01f);
	spot_light_dir_x_spinner_ = glui_->add_spinner_to_panel(light_panel, "Spot Light Dir X", GLUI_SPINNER_FLOAT, &spot_light_dir_x_live_var_, 5, ModelTransformCB_Static);
	spot_light_dir_x_spinner_->set_float_limits(-1.0f, 1.0f); spot_light_dir_x_spinner_->set_speed(0.1f);
	spot_light_dir_y_spinner_ = glui_->add_spinner_to_panel(light_panel, "Spot Light Dir Y", GLUI_SPINNER_FLOAT, &spot_light_dir_y_live_var_, 5, ModelTransformCB_Static);
    spot_light_dir_y_spinner_->set_float_limits(-1.0f, 1.0f); spot_light_dir_y_spinner_->set_speed(0.1f);
    spot_light_dir_z_spinner_ = glui_->add_spinner_to_panel(light_panel, "Spot Light Dir Z", GLUI_SPINNER_FLOAT, &spot_light_dir_z_live_var_, 5, ModelTransformCB_Static);
    spot_light_dir_z_spinner_->set_float_limits(-1.0f, 1.0f); spot_light_dir_z_spinner_->set_speed(0.1f);

	render_window_ = render_window;
    glui_->set_main_gfx_window(render_window);
}
