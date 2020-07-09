#pragma once
#include "stdafx.h"

// TODO universial material data & vertex data 

// Struct that store default vertex data
struct DefaultVertex 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoor;
};

// Describe the material data in mtl file
struct MaterialData 
{
	MaterialData() :
		Ka(0.2f, 0.2f, 0.2f),
		Kd(0.8f, 0.8f, 0.8f),
		Ks(1.0f, 1.0f, 1.0f),
		d(1.0f),
		Ns(0.0f),
		illum(1),
		map_Ka("")
	{}

	DirectX::XMFLOAT3 Ka;
	DirectX::XMFLOAT3 Kd;
	DirectX::XMFLOAT3 Ks;
	
	std::string map_Ka; // Texture file
	float d;
	float Ns;
	UINT illum; // illumination, 1 indicates no specular highlights
				//               2 denotes the presence of specular highlights so Ks is required
};

struct OBJIndex {
	UINT begin;
	UINT end;
};

// Read OBJ file
class MeshReader {

public:

	void ReadMaterial(const std::string& filePath);

	void ReadOBJ(const std::string& filePath);

	std::vector<DefaultVertex> m_vertex;
	std::vector<UINT> m_index;
	std::map<std::string, MaterialData> m_mtl;

	// Store the group information
	// group_index = m_group["group1"]
	// then [group_index.begin, group_index.end] stored in the index set are faces for this group
	std::map<std::string, OBJIndex> m_groupIndex;

	// Similar to m_group
	// TODO fix this, we can use one material mutiple times...
	std::map<std::string, OBJIndex> m_mtlIndex;

private:

	inline void ParseLine(std::vector<std::string>& tokens, const std::string& line, const char delim = ' ')
	{
		tokens.clear();
		std::string temp;
		std::stringstream instream(line);
		while (std::getline(instream, temp, delim))
		{
			tokens.push_back(temp);
		}
	}

	inline UINT GetIndex(const std::string& index, const UINT maxIndex) {
		int x = atoi(index.c_str());
		if (x > 0) {
			return(x - 1);
		}
		else {
			//TODO never test this
			return maxIndex + x;
		}
	}

	void CreateVertex(std::vector<DirectX::XMFLOAT3>& pos_list,
		std::vector<DirectX::XMFLOAT3>& nor_list,
		std::vector<DirectX::XMFLOAT2>& tex_list,
		const std::vector<std::string>& tokens);

	void CreateFace(
		const std::vector<DirectX::XMFLOAT3>& pos_list,
		const std::vector<DirectX::XMFLOAT3>& nor_list,
		const std::vector<DirectX::XMFLOAT2>& tex_list,
		const std::vector<std::string>& tokens);


};
