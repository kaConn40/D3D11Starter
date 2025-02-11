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
// For the DirectX Math library
using namespace DirectX;


// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
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
	
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
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
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	std::string s{"text"};
	// Feed fresh data to ImGui
	UpdateImGui(deltaTime, totalTime);
	//create window with requirements

	ImGui::Begin("Assignment Window");
	ImGui::Text("Framrate: %f fps", ImGui::GetIO().Framerate);
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", &color.x);
	/*ImGui::SliderInt("rate this ui ", slider.get(), 0, 100);
	ImGui::InputTextWithHint("Type ", "Any Feedback for the class so far", input.get(), 60);*/
	ImGui::ColorEdit4("cb colorTint", &colorTint.x);
	ImGui::SliderFloat3("offset", &offset.x,-1.0f,1.0f);

	for (int i = 0; i < meshList.size(); i++)
	{
		ImGui::Text("Shape %i Indices %i, Vertices: %i",i+1, meshList[i]->GetIndexCount(), meshList[i]->GetVertexCount());
	}
	

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
	verShaderData.offset = offset;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &verShaderData, sizeof(verShaderData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	for (auto& s :meshList)
	{
		s->Draw();
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



