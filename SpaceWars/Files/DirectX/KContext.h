#pragma once
#include <map>
#include "DXCore.h"
#include "InputReader.h"
#include "Model.h"
#include "ModelID.h"
#include "Shader.h"
#include "ShaderID.h"

namespace KDirectX {
	class KContext:DXCore {
		KInput::InputReader e;
		std::map<KModel::ModelID, KModel::Model> models;
		std::map<KShader::ShaderID, KShader::Shader> shaders;
	};
}