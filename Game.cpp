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
#include "WICTextureLoader.h"

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
		XMFLOAT3(-3, 2, -20.0f), //pos
		XM_PIDIV4, //45 degrees fov
		Window::AspectRatio(), //aspect ratio
		0.01f,  //near clip
		100.0f,  //far clip
		5.0f, //move speed
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
	shadowRes = 2048;
	shadowProjSize = 28.f;
	CreateShadowMap();



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
	XMFLOAT4 purple = XMFLOAT4(0.75f, 0, 0.6f, 1.0f);
	XMFLOAT4 grey = XMFLOAT4(.5, .5, .5, 1.0f);
	//create sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	{
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	}
	Graphics::Device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());
	//srvs
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>bronzeAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>bronzeMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>bronzeNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>bronzeRoughness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>woodAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>woodMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>woodNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>woodRoughness;


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>cobbleAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>cobbleMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>cobbleNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>cobbleRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>flatNormal;

	//load texture
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),FixPath(L"../../Assets/Textures/bronze_albedo.png").c_str(), 0, bronzeAlbedo.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_metal.png").c_str(), 0, bronzeMetal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_normals.png").c_str(), 0, bronzeNormal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_roughness.png").c_str(), 0, bronzeRoughness.GetAddressOf());

	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/wood_albedo.png").c_str(), 0, woodAlbedo.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/wood_metal.png").c_str(), 0, woodMetal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/wood_normals.png").c_str(), 0, woodNormal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/wood_roughness.png").c_str(), 0, woodRoughness.GetAddressOf());

	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/cobblestone_albedo.png").c_str(), 0, cobbleAlbedo.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/cobblestone_metal.png").c_str(), 0, cobbleMetal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str(), 0, cobbleNormal.GetAddressOf());
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/cobblestone_roughness.png").c_str(), 0, cobbleRoughness.GetAddressOf());

	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/flat_normals.png").c_str(), 0, flatNormal.GetAddressOf());
	

	//ps and vs
	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimpleVertexShader> skyVs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyVS.cso").c_str());

	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	std::shared_ptr<SimplePixelShader> skyPs = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"SkyPS.cso").c_str());

	std::shared_ptr<SimplePixelShader> envReflexPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"ReflectSkyPS.cso").c_str());

	shadowVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"ShadowVS.cso").c_str());
	
	




	
	
	std::shared_ptr<Mesh> cube=std::make_shared<Mesh>("Cube",FixPath("../../Assets/Models/cube.obj").c_str());
	std::shared_ptr<Mesh> cyl = std::make_shared<Mesh>("Cylinder", FixPath("../../Assets/Models/cylinder.obj").c_str());
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>("Helix", FixPath("../../Assets/Models/helix.obj").c_str());
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>("Quad", FixPath("../../Assets/Models/quad.obj").c_str());
	std::shared_ptr<Mesh> quadDS= std::make_shared<Mesh>("Double Sided Quad", FixPath("../../Assets/Models/quad_double_sided.obj").c_str());
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>("Sphere", FixPath("../../Assets/Models/sphere.obj").c_str());
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>("Torus", FixPath("../../Assets/Models/torus.obj").c_str());
	meshList.insert(meshList.begin(), { cube,cyl,helix,quad,quadDS,sphere, torus });

	

	//make sky
	sky = std::make_shared<Sky>(
		FixPath(L"../../Assets/Skies/Cold Sunset/right.png").c_str(),
		FixPath(L"../../Assets/Skies/Cold Sunset/left.png").c_str(),
		FixPath(L"../../Assets/Skies/Cold Sunset/up.png").c_str(),
		FixPath(L"../../Assets/Skies/Cold Sunset/down.png").c_str(),
		FixPath(L"../../Assets/Skies/Cold Sunset/front.png").c_str(),
		FixPath(L"../../Assets/Skies/Cold Sunset/back.png").c_str(),
		cube, skyPs, skyVs, sampler);


	std::shared_ptr<Material>bronzeMat = std::make_shared<Material>(ps, vs, white, .03f);
	bronzeMat->AddSampler("BasicSampler", sampler);
	bronzeMat->AddTextureSRV("Albedo", bronzeAlbedo);
	bronzeMat->AddTextureSRV("NormalMap", bronzeNormal);
	bronzeMat->AddTextureSRV("RoughnessMap", bronzeRoughness);
	bronzeMat->AddTextureSRV("MetalMap", bronzeMetal);


	std::shared_ptr<Material>woodMat = std::make_shared<Material>(ps, vs, white, .3f);
	woodMat->AddSampler("BasicSampler", sampler);
	woodMat->AddTextureSRV("Albedo", woodAlbedo);
	woodMat->AddTextureSRV("NormalMap", woodNormal);
	woodMat->AddTextureSRV("RoughnessMap", woodRoughness);
	woodMat->AddTextureSRV("MetalMap", woodMetal);


	std::shared_ptr<Material>cobbleMat = std::make_shared<Material>(ps, vs, white, .3f);
	cobbleMat->AddSampler("BasicSampler", sampler);
	cobbleMat->AddTextureSRV("Albedo", cobbleAlbedo);
	cobbleMat->AddTextureSRV("NormalMap", cobbleNormal);
	cobbleMat->AddTextureSRV("RoughnessMap", cobbleRoughness);
	cobbleMat->AddTextureSRV("MetalMap", cobbleMetal);



	matList.insert(matList.begin(), { bronzeMat,woodMat,cobbleMat});

	entityList.push_back(std::make_shared<GameEntity>(cube,bronzeMat ));
	entityList.push_back(std::make_shared<GameEntity>(sphere, woodMat));
	entityList.push_back(std::make_shared<GameEntity>(cyl, cobbleMat));
	entityList.push_back(std::make_shared<GameEntity>(helix, bronzeMat));
	entityList.push_back(std::make_shared<GameEntity>(torus, woodMat));
	entityList.push_back(std::make_shared<GameEntity>(quad, cobbleMat));
	entityList.push_back(std::make_shared<GameEntity>(quadDS, bronzeMat));


	float x = -12.0f;
	for (auto& e : entityList)
	{
		e->GetTransform()->MoveAbsolute(x, 0, 0);
		x += 3;
	}
	entityList.push_back(std::make_shared<GameEntity>(quad, woodMat));
	entityList.back()->GetTransform()->MoveAbsolute(0, -1.5, 0);
	entityList.back()->GetTransform()->Scale(50, 50, 50);

	
	//lights
	Light light1 = {};
	light1.Type = LIGHT_TYPE_DIRECTIONAL;
	light1.Color = XMFLOAT3(1.0,1.0,1.0);
	light1.Direction = XMFLOAT3(0, -1, 0);
	light1.Intensity = 1.0f;

	Light light2 = {};
	light2.Type = LIGHT_TYPE_DIRECTIONAL;
	light2.Color = XMFLOAT3(1.0, 1.0, 1.0);
	light2.Direction = XMFLOAT3(0, -1, 0);
	light2.Intensity = 1.0f;

	Light light3 = {};
	light3.Type = LIGHT_TYPE_DIRECTIONAL;
	light3.Color = XMFLOAT3(1.0, 1.0, 1.0);
	light3.Direction = XMFLOAT3(0, 0, -1);
	light3.Intensity = .0f;
	

	Light light4 = {};
	light4.Type = LIGHT_TYPE_POINT;
	light4.Color = XMFLOAT3(1.0,1.0, 1.0);
	light4.Position = XMFLOAT3(-1.5, 0, 0);
	light4.Intensity = 0.0f;
	light4.Range = 8.0f;

	Light light5 = {};
	light5.Type = LIGHT_TYPE_SPOT;
	light5.Color = XMFLOAT3(1.0, 1.0, 1.0);
	light5.Position = XMFLOAT3(3.2f, .5f,.2f );
	light5.Direction=XMFLOAT3(-1, 0, 0);
	light5.Range = 40;
	light5.SpotInnerAngle = XMConvertToRadians(30.0f);
	light5.SpotOuterAngle = XMConvertToRadians(20.0f);
	light5.Intensity = 0.0f;

	lights.push_back(light1);
	/*lights.push_back(light2);
	lights.push_back(light3);
	lights.push_back(light4);
	lights.push_back(light5);*/
		for (int i = 0; i < lights.size(); i++)
		if (lights[i].Type != LIGHT_TYPE_POINT)
			XMStoreFloat3(
				&lights[i].Direction, 
				XMVector3Normalize(XMLoadFloat3(&lights[i].Direction))
			);




	
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

	if (ImGui::TreeNode("Lights"))
	{
		int count = 1;
		for (auto& l:lights)
		{
			
			ImGui::PushID(&l);
			std::string lightType;
			switch (l.Type)
			{
			case LIGHT_TYPE_DIRECTIONAL:
				lightType = "Directional";break;
			case LIGHT_TYPE_POINT: lightType = "Point"; break;
			case LIGHT_TYPE_SPOT: lightType = "Spot"; break;
			}
			
			if (ImGui::TreeNode("Lights", "light %i %s", count, lightType.c_str()))
			{
				
				if (l.Type == LIGHT_TYPE_DIRECTIONAL || l.Type == LIGHT_TYPE_SPOT)
				{
					ImGui::DragFloat3("Direction", &l.Direction.x, .1f);
					XMVECTOR dirNorm = XMVector3Normalize(XMLoadFloat3(&l.Direction));
					XMStoreFloat3(&l.Direction, dirNorm);
				
				}
				if (l.Type == LIGHT_TYPE_POINT || l.Type == LIGHT_TYPE_SPOT)
				{
					ImGui::DragFloat3("Pos", &l.Position.x, 0.1f);
					ImGui::SliderFloat("Range", &l.Range, .1f, 50.0f);
					

				}

				
				
			
					ImGui::ColorEdit3("light Color", &l.Color.x);
					ImGui::ColorEdit3("Ambient Color", &ambientColor.x);
					ImGui::SliderFloat("Intensity", &l.Intensity, 0, 20.f);

				
					ImGui::TreePop();
			}
				ImGui::PopID();

				count++;

		}
		ImGui::TreePop();
	}

	ImGui::Image((ImTextureID)shadowSRV.Get(), ImVec2(512, 512));
		

	
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
	for (int i=0;i<entityList.size()-1;i++)
	{
		entityList[i]->GetTransform()->Rotate(0, deltaTime, 0);
	}
	activeCam->Update(deltaTime);
	entityList[2]->GetTransform()->SetPosition((float)sin(totalTime)*2, 0, -6);

	entityList[1]->GetTransform()->SetPosition(entityList[1]->GetTransform()->GetPosition().x, (float)sin(totalTime), 0);
	


	
	
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	&color.x);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	//enable raster
	Graphics::Context->RSSetState(shadowRasterizer.Get());

	RenderShadowMap();
	Graphics::Context->RSSetState(0);
	

	for (auto& s : entityList)
	{
		std::shared_ptr<SimpleVertexShader> vs = s->GetMaterial()->GetVertexShader();
		vs->SetMatrix4x4("lightView", lightViewMatrix);
		vs->SetMatrix4x4("lightProjection", lightProjectionMatrix);

		std::shared_ptr<SimplePixelShader> ps = s->GetMaterial()->GetPixelShader();
		ps->SetFloat3("ambient", ambientColor);
		ps->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		ps->SetShaderResourceView("ShadowMap", shadowSRV);
		ps->SetSamplerState("ShadowSampler", shadowSampler);
		s->Draw(activeCam);
	}
	sky->Draw(activeCam);
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

		ID3D11ShaderResourceView* nullSRVs[128] = {};
		Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);
	}
}

void Game::CreateShadowMap()
{
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	{
		shadowDesc.Width = shadowRes; // Ideally a power of 2 (like 1024)
		shadowDesc.Height = shadowRes; // Ideally a power of 2 (like 1024)
		shadowDesc.ArraySize = 1;
		shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowDesc.MipLevels = 1;
		shadowDesc.MiscFlags = 0;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	{
		shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		shadowDSDesc.Texture2D.MipSlice = 0;

	}
	Graphics::Device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	{
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
	}
	Graphics::Device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV.GetAddressOf());

	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	{
		shadowRastDesc.FillMode = D3D11_FILL_SOLID;
		shadowRastDesc.CullMode = D3D11_CULL_BACK;
		shadowRastDesc.DepthClipEnable = true;
		shadowRastDesc.DepthBias = 100; // Min. precision units, not world units!
		shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	}
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_SAMPLER_DESC shadowSampDesc = {};
	{
		shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	}
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);


	XMMATRIX lightView = XMMatrixLookAtLH(
		XMVectorSet(0, 20, -20, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&lightViewMatrix, lightView);

	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		shadowProjSize,
		shadowProjSize,
		1.0f,
		100.0f);
	XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);
}

void Game::RenderShadowMap()
{
	//Clear ShadowMap
	Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//output merger
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());

	Graphics::Context->PSSetShader(0, 0, 0);
	//deactivate viewport
	D3D11_VIEWPORT viewport = {};
	{
		viewport.Width = (float)shadowRes;
		viewport.Height = (float)shadowRes;
		viewport.MaxDepth = 1.0f;
	}
	Graphics::Context->RSSetViewports(1, & viewport);

	//Ent render loop
	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", lightViewMatrix);
	shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);

	// Loop and draw all entities
	for (auto& e : entityList)
	{
		shadowVS->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
		shadowVS->CopyAllBufferData();
		// Draw the mesh directly to avoid the entity's material
		// Note: Your code may differ significantly here!
		e->GetMesh()->Draw();
	}

	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
}



