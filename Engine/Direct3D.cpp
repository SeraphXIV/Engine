// Include du header associe
#include "Direct3D.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CDirect3D::CDirect3D()
{
	// Mise a zero des variables et pointeurs
	m_device = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}
// On ne s'en servira pas
CDirect3D::CDirect3D(const CDirect3D& other){}
CDirect3D::~CDirect3D(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CDirect3D::Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
			  float screenDepth, float screenNear)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CDirect3D::Init()");
	#endif

	// Declaration des variables
	HRESULT result; // bool special Windows
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D10Texture2D* backBufferPtr;
	D3D10_TEXTURE2D_DESC depthBufferDesc;
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D10_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D10_RASTERIZER_DESC rasterDesc;
	D3D10_DEPTH_STENCIL_DESC depthDisabledStencilDesc;


	// Sauvegarde des options de sync verticale
	m_vsync_enabled = vsync;

	// Partie pour detecter les meilleurs options selon les cartes graphiques et les ecrans

	// Cree une "DirectX graphics interface factory"
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to Create DXGI Factory."); 
		#endif
		return false;}

	// Utilise le factory pour creer un adaptateur vers l'interface graphique principale (la carte graphique en gros)
	result = factory->EnumAdapters(0, &adapter);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to enum Factory adapters."); 
		#endif
		return false;}

	// Enumere les sorties d'adaptateurs (les ecrans en gros)
	result = adapter->EnumOutputs(0, &adapterOutput);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to enum Adapters outputs."); 
		#endif
		return false;}

	// Accesseur du nombre de mode qui supporte le format R8G8B8A8_UNORM (32bit) pour le(s) ecran(s)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to Get Display Mode List"); 
		#endif
		return false;}

	// Sauvegarde des modes d'affichages possibles pour cette combinaison CG/ecran
	displayModeList = new DXGI_MODE_DESC[numModes];
	// Test de succes
	if(!displayModeList){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to init displayModeList."); 
		#endif
		return false;}

	// Remplissage de la structure d'affichage avec les infos acquises
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to fill Display Mode List."); 
		#endif
		return false;}

	// Parcours de tout les modes d'affichages valides pour trouver celui qui coincide avec la resolution voulue
	// Quand une occurence valide est trouvee, on sauvegarde le taux de rafraichissement de l'ecran
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Maintenant qu'on a le taux de raifraichissement, on cherche le nom de la CG et surtout la memoire dont elle dispose

	// Accesseur de la description de la CG
	result = adapter->GetDesc(&adapterDesc);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to get graphic card desc."); 
		#endif
		return false;}

	// Sauvegarde de la memoire dedicacee de la CG en megabytes
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Converti le nom de la CG en un tableau de char et le sauvergarde
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	// Test de succes
	if(error != 0){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to convert graphic card name."); 
		#endif
		return false;}

	// Maintenant qu'on a toute les infos sur le CG et l'ecran, on release tout les accesseurs
	delete [] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	// On peux commencer l'initialisation de Direct3D 10

	// A commencer par la swap chain : ensemble de buffer, pour nous 2 : front buffer et back buffer
	// La scene est calculee et dessinee sur le backbuffer, puis on inverse les deux buffers
	// le back devient le front et est affiche a l'ecran, le front devient le back et on dessine dessus la prochaine scene
	// Ca evite les scintillements quand l'image se rafraichie

	// Mise a zero de la description de la swapchain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Un seul backbuffer, ca suffit largement
	swapChainDesc.BufferCount = 1;

	// Resolution du backbuffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Surface 32bit pour le backbuffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Mise en place du taux de rafraichissement
	// Si le V-Sync est activé, le FPS sera celui donne par l'ecran plus haut
	// Sinon, le programme tentera d'avoir le FPS le plus haut possible, ce qui n'est pas bon
	// Ca peux causer des trucs bizarres a l'affichage, on va eviter, mais l'option existe

	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Defini l'usage qu'on veux faire du backbuffer, dessiner dessus
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Donne la fenetre sur laquelle on veux dessiner
	swapChainDesc.OutputWindow = hwnd;

	// Met le multisampling sur off
	// /!\ Aucune idee de ce que c'est
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Defini si fullscreen ou pas
	if(fullscreen){swapChainDesc.Windowed = false;}
	else{swapChainDesc.Windowed = true;}

	// Defini le "scan line ordering" et le "scaling" sur non-specifie
	// /!\ Aucune idee de ce que c'est
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Active le nettoyage du backbuffer après son affichage
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Aucun flag avance
	// /!\ Aucune idee de ce que c'est
	swapChainDesc.Flags = 0;

	// Maintenant que tout est configure, on peux creer notre swapchain et notre Device.
	// Le Device est l'interface qui nous permettra d'appeler les fonctions Direct3D
	result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, 
					       &swapChainDesc, &m_swapChain, &m_device);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to Create Device And Swap Chain."); 
		#endif
		return false;}

	// Creation et liaison d'un backbuffer a la swapchain
	// Acces au pointeur vers le backbuffer de la swapchain
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to access to backbuffer."); 
		#endif
		return false;}

	// Creation de la render target view avec le pointeur vers le backbuffer
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to Create Render Target View."); 
		#endif
		return false;}

	// Release du pointeur vers le backbuffer, tout est lie on en a plus besoin
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Mise en place du Depth Buffer, qui permet la profondeur dans la scene, la 3D

	// Mise a zero
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Description du Depth buffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL; // On precise qu'on y attachera un depth stencil, qui permet des effets graphiques plus poussés (motion blur, volumetric shadow, etc...)
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Creation du depth et depth stencil buffers
	// On remarquera que ce qui nous permet la 3D n'est en fin de compte qu'une texture 2D
	// Apres traitement et applatissement (rasterization), on obtiens bel et bien qu'une texture 2D a afficher
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to create depth buffer."); 
		#endif
		return false;}

	// Mise en place du depth stencil, ca nous permet de controler le traitement de profondeur
	// que fera DirectX a chaque pixel

	// Mise a zero
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Description du Depth stencil
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Operation stencil si un pixel est de face
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Operation stencil si un pixel est de dos
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Creation du depth stencil state
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to create 3D depth stencil state."); 
		#endif
		return false;}
	// Liaison du depth stencil state au D3D device
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);

	// Mise en place du Depth stencil view, qui permet a D3D de considerer le depth buffer comme un depth stencil buffer

	// Mise a zero
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Description du Depth stencil view
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Creation du depth stencil view
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to create depth stencil view."); 
		#endif
		return false;}

	// Bind le tout au pipeline d'affichage
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Mise en place de la rasterization, DirectX en met une par defaut, on va decrire la notre
	// Le raster permet un control sur l'affichage des polygones (wireframe, etc...)
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D10_CULL_BACK; // Suppression des faces qui sont dos a la cam, l'inverse est possible (CULL_FACE)
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D10_FILL_SOLID; // Ici pour le wireframe
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Creation du rasterizer state
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to create rasterizer state."); 
		#endif
		return false;}
	// Envoi au device
	m_device->RSSetState(m_rasterState);

	// Mise en place du viewport, ce qui deborde du viewport sera supprime
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Creation du viewport
	m_device->RSSetViewports(1, &viewport);

	// Mise en place de la matrice de projection, celle qui permet d'"ecraser" le monde 3D pour en faire un monde 2D affichable
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;
	// Creation de la matrice de projection
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// Mise en place de la matrice du monde, celle a manipuler pour bouger les objets 3D
	D3DXMatrixIdentity(&m_worldMatrix);

	// Creation de la matrice orthographique, pour l'affichage de choses en 2D (interface, etc)
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// Met le second depth stencil state a zero avant de le remplir
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Cree un second depth stencil state qui desactive Z pour dessiner en 2D
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Cree le second depth stencil state
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CDirect3D::Init() : Failed to create 2D depth stencil state."); 
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CDirect3D::Shutdown()
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CDirect3D::Shutdown()");
	#endif

	// Avant de tout detruire, il faut imperativement revenir au mode fenetre, sinon la swapchain plante
	if(m_swapChain){m_swapChain->SetFullscreenState(false, NULL);}

	// Release des classes et remise a zero des pointeurs
	if(m_rasterState){
		m_rasterState->Release();
		m_rasterState = 0;}

	if(m_depthStencilView){
		m_depthStencilView->Release();
		m_depthStencilView = 0;}

	if(m_depthStencilState){
		m_depthStencilState->Release();
		m_depthStencilState = 0;}

	if(m_depthDisabledStencilState){
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;}

	if(m_depthStencilBuffer){
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;}

	if(m_renderTargetView){
		m_renderTargetView->Release();
		m_renderTargetView = 0;}

	if(m_swapChain){
		m_swapChain->Release();
		m_swapChain = 0;}

	if(m_device){
		m_device->Release();
		m_device = 0;}
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Aide au render																//
//////////////////////////////////////////////////////////////////////////////////////////
void CDirect3D::BeginScene(float red, float green, float blue, float alpha)
{
	// S'occupe de tout ce qu'il faut faire avant de dessiner une scene

	float color[4];
	// Couleur pour remplir le buffer
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Nettoie les buffers
	m_device->ClearRenderTargetView(m_renderTargetView, color);
	m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void CDirect3D::EndScene()
{
	// S'occupe de tout ce qu'il faut faire apres avoir dessiner une scene

	// Affiche le backbuffer a l'ecran une fois que le dessin est fini
	if(m_vsync_enabled){
		// Verouille le FPS si la V-Sync est activee
		m_swapChain->Present(1, 0);}
	else{
		// Affiche le plus vite possible sinon
		m_swapChain->Present(0, 0);}
}

// Swap d'un depth stencil state a l'autre
void CDirect3D::TurnZBufferOn(){m_device->OMSetDepthStencilState(m_depthStencilState, 1);}
void CDirect3D::TurnZBufferOff(){m_device->OMSetDepthStencilState(m_depthDisabledStencilState, 1);}

//////////////////////////////////////////////////////////////////////////////////////////
//			Acces au Device																//
//////////////////////////////////////////////////////////////////////////////////////////
ID3D10Device* CDirect3D::GetDevice()
{
	// Le device etant l'interface d'acces a tout le reste de direct3D, on va avoir
	// besoin d'y avoir acces d'ailleurs
	return m_device;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Acces aux matrices															//
//////////////////////////////////////////////////////////////////////////////////////////
// Les shaders vont avoir besoin d'un acces aux matrices, ou au moins a des copies
void CDirect3D::GetProjectionMatrix(D3DXMATRIX& out_projectionMatrix){
	out_projectionMatrix = m_projectionMatrix;
	return;}
void CDirect3D::GetWorldMatrix(D3DXMATRIX& out_worldMatrix){
	out_worldMatrix = m_worldMatrix;
	return;}
void CDirect3D::GetOrthoMatrix(D3DXMATRIX& out_orthoMatrix){
	out_orthoMatrix = m_orthoMatrix;
	return;}

//////////////////////////////////////////////////////////////////////////////////////////
//			Acces aux infos CG															//
//////////////////////////////////////////////////////////////////////////////////////////
void CDirect3D::GetVideoCardInfo(char* out_cardName, int& out_memory)
{
	strcpy_s(out_cardName, 128, m_videoCardDescription);
	out_memory = m_videoCardMemory;
	return;
}