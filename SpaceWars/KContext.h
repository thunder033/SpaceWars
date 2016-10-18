#pragma once
#include <map>
#include "DirectX\DXCore.h"
#include "Input\InputReader.h"
#include "Graphic\Mesh.h"
#include "Graphic\MeshID.h"
#include "Graphic\Shader.h"
#include "Graphic\ShaderID.h"

namespace NDirectX {
	class KContext: DXCore {
		KInput::InputReader e;
		std::map<KModel::Mesh, KModel::MeshId> models;
		std::map<KShader::ShaderID, KShader::Shader> shaders;
	};
}