//#pragma once
//
//#include "Transform.h"
//#include "SimpleMath.h"
//#include <vector>
//
//using namespace DirectX::SimpleMath;
//
//struct OBB {
//
//	Vector3 r;
//	Vector3 s;
//	Vector3 t;
//
//	Vector3 c;
//	Matrix u;
//	Vector3 e;
//
//	std::vector<Vector3> verts;
//
//	std::vector<Vector3> GetWorldVerts() {
//		std::vector<Vector3> worldVerts;
//		std::vector<Vector3>::iterator it;
//		for (it = verts.begin(); it != verts.end(); ++it) {
//			worldVerts.push_back((*it) + c);
//		}
//		return worldVerts;
//	}
//};