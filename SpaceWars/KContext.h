#pragma once
#include <map>
#include "DXCore.h"
#include "InputReader.h"
#include "KGraphic/Mesh.h"
#include "KGraphic/MeshID.h"
#include "Shader.h"
#include "ShaderID.h"

namespace KDirectX {
	class KContext:DXCore {
		KInput::InputReader e;
		std::map<KModel::Mesh, KModel::MeshId> models;
		std::map<KShader::ShaderID, KShader::Shader> shaders;
	};
}