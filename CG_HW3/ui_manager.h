#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "headers.h"
#include "trianglemesh.h"
#include "skybox.h"
#include "light.h"
#include "glui.h"
// Windows headers.
#include <windows.h>
#include <commdlg.h>

class UIManager {
public:
	UIManager();

	int GetRenderWindow() const { return render_window_; }
	float GetScaleLiveVar() const { return scale_live_var_; }
	float* GetTranslateXYLiveVar() { return translate_xy_live_var_; }
	float* GetTranslateZLiveVar() { return translate_z_live_var_; }
	float* GetRotateMatrixLiveVar() { return rotate_matrix_live_var_; }
	int GetUseSkybox() const { return use_skybox_checkbox_->get_int_val(); }
	bool GetShowLight() const { return show_light_;}

	void SetModelList(std::vector<TriangleMesh*>* model_list) { model_list_ = model_list; }
	void SetSelectedModel(TriangleMesh* model) { selected_model_ = model; }
	void SetPointLightList(std::vector<PointLight*>* point_light_list) { point_light_list_ = point_light_list; }
	void SetSpotLightList(std::vector<SpotLight*>* spot_light_list) { spot_light_list_ = spot_light_list; }
	void SetSelectedPointLight(PointLight* point_light) { selected_point_light_ = point_light; }
	void SetSelectedSpotLight(SpotLight* spot_light) { selected_spot_light_ = spot_light; }
	void SetSkybox(Skybox* skybox) { skybox_ = skybox; }

	void ChooseModelCB(int state);
	void AddOrDeleteModel(bool deletion, const int& id, std::string file_path = "");
	void AddOrDeleteLight(bool deletion, const int& id, const int& type);
	void LoadModelButtonCB(int state);
	void LoadSkyboxButtonCB(int state);
	void RenderModeButtonCB(int state);
	void ModelTransformCB(int state);
	void ResetScene();
	void ResetSceneButtonCB(int state);

	// Static callback wrappers
	static void ChooseModelCB_Static(int state);
	static void LoadModelButtonCB_Static(int state);
	static void LoadSkyboxButtonCB_Static(int state);
	static void RenderModeButtonCB_Static(int state);
	static void ModelTransformCB_Static(int state);
	static void ResetSceneButtonCB_Static(int state);

	static void LightOnOffButtonCB_Static(int state);

	void InitGLUI(const int &render_window, const int& screenWidth);

private:
	static UIManager* instance_;
	
	// GLUI.
	GLUI* glui_;
	int render_window_;
	GLUI_RadioGroup* render_mode_radio_group_ = nullptr;
	GLUI_Spinner* scale_spinner_ = nullptr;
	GLUI_Translation* xy_translator_ = nullptr;
	GLUI_Translation* z_translator_ = nullptr;
	GLUI_Translation* point_light_xy_translator_ = nullptr;
	GLUI_Translation* point_light_z_translator_ = nullptr;
	GLUI_Translation* spot_light_xy_translator_ = nullptr;
	GLUI_Translation* spot_light_z_translator_ = nullptr;
	GLUI_Spinner* spot_light_dir_x_spinner_ = nullptr;
	GLUI_Spinner* spot_light_dir_y_spinner_ = nullptr;
	GLUI_Spinner* spot_light_dir_z_spinner_ = nullptr;
	GLUI_Checkbox* use_skybox_checkbox_ = nullptr;
	GLUI_Checkbox* rotate_skybox_checkbox_ = nullptr;
	GLUI_Rotation* rotator_ = nullptr;
	GLUI_Listbox* model_list_box_ = nullptr;
	GLUI_Listbox* point_light_list_box_ = nullptr;
	GLUI_Listbox* spot_light_list_box_ = nullptr;
	float scale_live_var_;
	float translate_xy_live_var_[2]; // Translation values for XY plane.
	float translate_z_live_var_[1];        // Translation value for Z axis.
	float point_light_translate_xy_live_var_[2]; // Point Light source translation values for XY plane.
	float point_light_translate_z_live_var_[1];  // Point Light source translation value for Z axis.
	float spot_light_translate_xy_live_var_[2]; // Spot Light source translation values for XY plane.
	float spot_light_translate_z_live_var_[1];  // Spot Light source translation value for Z axis.
	float spot_light_dir_x_live_var_;      // Spot Light source direction X component.
	float spot_light_dir_y_live_var_;      // Spot Light source direction Y component.
	float spot_light_dir_z_live_var_;      // Spot Light source direction Z component.
	bool show_light_;
	float rotate_matrix_live_var_[16]; // 4x4 rotation matrix (Column-major order)

	// For model management.
	std::vector<TriangleMesh*>* model_list_;
	TriangleMesh* selected_model_;

	std::vector<PointLight*>* point_light_list_;
	std::vector<SpotLight*>* spot_light_list_;
	PointLight* selected_point_light_;
	SpotLight* selected_spot_light_;
	// DirectionalLight* selected_directional_light_;

	Skybox* skybox_;

	
}; 

#endif