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
	LoadShaders();
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

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);

		unsigned int size = sizeof(constBuffer);
		size = (size + 15) / 16 * 16;
		D3D11_BUFFER_DESC cbDesc = {}; // Sets struct to all zeros
		{
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.ByteWidth = size; // Must be a multiple of 16
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;
		}
		
		Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());
		Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
		// Initialize ImGui itself & platform/renderer backend
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
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
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
	XMFLOAT4 white= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.f, 1.0f, 0.4f, 1.0f);
	//Mesh A
	unsigned int indicesA[] = { 0, 1, 2 };
	Vertex verticesA[] =
	{
		{ XMFLOAT3(+0.5f, +0.3f, +0.0f), red },
		{ XMFLOAT3(+0.8f, -0.3f, +0.0f), blue },
		{ XMFLOAT3(+0.2f, -0.3f, +0.0f), green },
	};

	//Mesh B
	Vertex verticesB[] =
	{
		{XMFLOAT3(-0.85f,+0.85f,+0.0f), blue},
		{ XMFLOAT3(-0.85f,+0.60f,+0.0f), blue },
		{XMFLOAT3(-0.60f,+0.60f,+0.0f), blue},
		{XMFLOAT3(-0.60f,+0.85f,+0.0f), blue},
	};
	unsigned int indicesB[] =
	{
		0,2,1,
		0,3,2
	};

	//Mesh C
	Vertex verticesC[]
	{
		{ XMFLOAT3(-0.20f, +0.3f, +0.0f), yellow },//0
		{ XMFLOAT3(-0.10f,-0.00f,+0.0f), yellow },//1
		{ XMFLOAT3(-0.30f,-0.00f,+0.00f), yellow  },//2

		{ XMFLOAT3(+0.15f,-0.00f,+0.00f), yellow  }, //3
		{ XMFLOAT3(-0.1f,-0.15f,+0.00f), yellow  },//4
		{ XMFLOAT3(+0.050f,-0.40f,+0.00f), yellow  },//5
		{ XMFLOAT3(-0.2f,-0.20f,+0.00f), yellow  },//6


		{ XMFLOAT3(-0.40f,-0.40f,+0.00f), yellow  },//7
		{ XMFLOAT3(-0.30f,-0.15f,+0.00f), yellow  },//8

		{ XMFLOAT3(-0.5f,-0.00f,+0.00f), yellow  }, //9
		
	};
	unsigned int indicesC[] =
	{
		0,1,2,
		1,3,4,
		6,4,5,
		7,8,6,
		2,8,9,
		8,2,4,
		4,2,1,
		6,8,4

	
	};
	std::shared_ptr<Mesh> shapeA=std::make_shared<Mesh>((int)ARRAYSIZE(verticesA), (int)ARRAYSIZE(indicesA), verticesA, indicesA);
	std::shared_ptr<Mesh> shapeB = std::make_shared<Mesh>((int)ARRAYSIZE(verticesB), (int)ARRAYSIZE(indicesB), verticesB, indicesB);
	std::shared_ptr<Mesh> shapeC = std::make_shared<Mesh>((int)ARRAYSIZE(verticesC), (int)ARRAYSIZE(indicesC), verticesC, indicesC);

	
	meshList.push_back(shapeA);
	meshList.push_back(shapeB);
	meshList.push_back(shapeC);

	std::shared_ptr<GameEntity>ent1 = make_shared<GameEntity>(shapeA);
	std::shared_ptr<GameEntity>ent2 = make_shared<GameEntity>(shapeB);
	std::shared_ptr<GameEntity>ent3 = make_shared<GameEntity>(shapeB);
	std::shared_ptr<GameEntity>ent4 = make_shared<GameEntity>(shapeC);
	std::shared_ptr<GameEntity>ent5 = make_shared<GameEntity>(shapeC);

	ent3->GetTransform()->MoveAbsolute(1.0f, 0.0f, 0.0f);
	ent5->GetTransform()->MoveAbsolute(-.6f, -.4f, 0.0f);

	entityList.push_back(ent1);
	entityList.push_back(ent2);
	entityList.push_back(ent3);
	entityList.push_back(ent4);
	entityList.push_back(ent5);

	
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
	constBuffer verShaderData;
	verShaderData.colorTint = colorTint;

	for (auto& s : entityList)
	{
		s->Draw(constantBuffer, verShaderData, activeCam);
	}
	
	{
		

		ImGui::Render(); // Turns this frame�s UI into renderable triangles
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



