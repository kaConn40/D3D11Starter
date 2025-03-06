#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include <DirectXMath.h>


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "imgui.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <iostream>
#include <format> 
#include "SimpleShader.h"
#include"Material.h"
// For the DirectX Math library
using namespace DirectX;


// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	
	//set up cameras
	std::shared_ptr<Camera>cam1 = std::make_shared<Camera>(
		XMFLOAT3(0, 0, -7.0f), //pos
		XM_PIDIV4, //45 degrees fov
		Window::AspectRatio(), //aspect ratio
		0.01f,  //near clip
		100.0f,  //far clip
		3.0f, //move speed
		.002f, //mouse speed
		true //is active
		);
	std::shared_ptr<Camera>cam2=std::make_shared<Camera>(
		XMFLOAT3(1.5, 0, -3.0f), //pos
		XM_PIDIV2, //90 degrees fov
		Window::AspectRatio(), //aspect ratio
		0.01f,  //near clip
		100.0f,  //far clip
		3.0f, //move speed
		.002f, //mouse speed
		false //is active
	);
	std::shared_ptr<Camera>cam3 = std::make_shared<Camera>(
		XMFLOAT3(0, 2, -5.0f), //pos
		XM_PIDIV2, //90 degrees fov
		Window::AspectRatio(), //aspect ratio
		0.01f,  //near clip
		100.0f,  //far clip
		3.0f, //move speed
		.002f, //mouse speed
		false //is active
	);
	cams.push_back(cam1);
	cams.push_back(cam2);
	cams.push_back(cam3);
	//hold which ever cam is being used and let us change it

	activeCam = cam1;
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();
	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}





// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	//Colors
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.f, 1.0f, 0.4f, 1.0f); 

	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());

	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	std::shared_ptr<Material> mat1 = std::make_shared<Material>(ps, vs, red);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(ps, vs, blue);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(ps, vs, green);







	//std::cout << FixPath("Assets/Models/cube.obj").c_str() << std::endl;
	
	//std::shared_ptr<Mesh> cube=std::make_shared<Mesh>("Cube",FixPath("../../Assets/Models/cube.obj").c_str());
	//std::shared_ptr<Mesh> cyl = std::make_shared<Mesh>("Cylinder", FixPath("../../Assets/Models/cylinder.obj").c_str());
	//std::shared_ptr<Mesh> helix = std::make_shared<Mesh>("Helix", FixPath("../../Assets/Models/sphere.obj").c_str());
	//std::shared_ptr<Mesh> quad = std::make_shared<Mesh>("Quad", FixPath("../../Assets/Models/sphere.obj").c_str());
	//std::shared_ptr<Mesh> quadDS= std::make_shared<Mesh>("Double Sided Quad", FixPath("../../Assets/Models/sphere.obj").c_str());
	//std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>("Sphere", FixPath("../../Assets/Models/sphere.obj").c_str());
	//std::shared_ptr<Mesh> torus = std::make_shared<Mesh>("Torus", FixPath("../../Assets/Models/sphere.obj").c_str());

	


	
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (auto& c:cams)
	{
		c->UpdateProjectionMatrix(Window::AspectRatio());
	}
	
}
/// <summary>
/// helper method for game update
/// updates all things related to ImGui that need to be updated frame to frame
/// </summary>
void Game::UpdateImGui(float deltaTime,float totalTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	//create window with requirements

	ImGui::Begin("Assignment Window");
	ImGui::Text("Framrate: %f fps", ImGui::GetIO().Framerate);
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", &color.x);
	ImGui::ColorEdit4("cb colorTint", &colorTint.x);
	

	if (ImGui::TreeNode("Entites"))
	{
		for (int i = 0; i < entityList.size(); i++)
		{
			XMFLOAT3 pos = entityList[i]->GetTransform()->GetPosition();
			XMFLOAT3 rot = entityList[i]->GetTransform()->GetPitchYawRoll();
			XMFLOAT3 scl = entityList[i]->GetTransform()->GetScale();
			ImGui::PushID(entityList[i].get());

			if (ImGui::TreeNode("Entity Node", "Entity %i", i + 1))
			{
				ImGui::DragFloat3("Position", &pos.x, .01f);
				ImGui::DragFloat3("Rotation", &rot.x, .01f);
				ImGui::DragFloat3("scale", &scl.x, .01f);
				ImGui::TreePop();
			}
			ImGui::PopID();

			entityList[i]->GetTransform()->SetPosition(pos);
			entityList[i]->GetTransform()->SetRotation(rot);
			entityList[i]->GetTransform()->SetScale(scl);

		}
		ImGui::TreePop();
	}
	//create buttons to switch cameras
	std::string s;
	for(int i=0;i<cams.size();i++)
	{
		ImGui::PushID(cams[i].get());
		s = std::format("Camera {}", i + 1);
		if (ImGui::RadioButton(s.c_str(), cams[i]->isActive))
		{
			activeCam.get()->isActive=false;
			activeCam=cams[i];
			activeCam.get()->isActive = true;
		}
		if(i<cams.size()-1) ImGui::SameLine();
		ImGui::PopID();
	}
	
		
		

	
	//set the info up and let it be changed by ui
	XMFLOAT3 pos = activeCam->GetTransform()->GetPosition();
	XMFLOAT3 rot = activeCam->GetTransform()->GetPitchYawRoll();
	float fov = activeCam->GetFOV();
	if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
		activeCam->GetTransform()->SetPosition(pos);

	if (ImGui::DragFloat3("Rotation (Radians)", &rot.x, 0.01f))
		activeCam->GetTransform()->SetRotation(rot);

	if (ImGui::DragFloat("FOV", &fov, 0.01f, XM_PIDIV4,XM_PIDIV2))
		activeCam->SetFOV(fov);


	if (ImGui::Button("Open/Close demoWindow"))
	{
		demoUI = !demoUI;
	}
	if (demoUI)
	{
		ImGui::ShowDemoWindow();
	}
	ImGui::CloseCurrentPopup();
	ImGui::End();
	// Show the demo window
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	
	// Feed fresh data to ImGui
	UpdateImGui(deltaTime, totalTime);
	//check to see if camera changed

	activeCam->Update(deltaTime);
	

	//had to total time or it didnt work
	float scale = ((float)cos(totalTime * 3) /2.0f);
	entityList[3]->GetTransform()->SetScale(scale, scale, scale); 
	entityList[2]->GetTransform()->Rotate(0, 0, (float)sin(deltaTime*.5f));
	entityList[0]->GetTransform()->SetPosition((float)cos(totalTime)/2, (float)sin(totalTime)/2, 0);


	
	
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	&color.x);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	//give the cb new data 
	

	for (auto& s : entityList)
	{
		s->Draw(activeCam);
	}
	
	{
		

		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
	}


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



