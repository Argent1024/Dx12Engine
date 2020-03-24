#pragma once
#include "stdafx.h"

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

namespace MeshReader
{
	namespace // Helper functions
	{
		void ParseLine(std::vector<std::string>& tokens, const std::string& line, const char delim = ' ')
		{
			tokens.clear();
			std::string temp;
			std::stringstream instream(line);
			while (std::getline(instream, temp, delim))
			{
				tokens.push_back(temp);
			}
		}

		void CreateVertex(std::vector<DirectX::XMFLOAT3>& pos_list,
						std::vector<DirectX::XMFLOAT3>& nor_list,
						std::vector<DirectX::XMFLOAT2>& tex_list,
						const std::vector<std::string>& tokens)
		{
			const std::string type = tokens[0];
			
			if (type.compare("v") == 0) {
			
				float x = std::stof(tokens[1].c_str());
				float y = std::stof(tokens[2].c_str());
				float z = std::stof(tokens[3].c_str());
				//float w = 0.0f;
				// contains w
				//if (tokens.size() == 5) { float w = atof(tokens[4].c_str()); }
				pos_list.push_back(DirectX::XMFLOAT3(x, y, z));
				
			} else if (type.compare("vn") == 0) {
	
				float x = std::stof(tokens[1].c_str());
				float y = std::stof(tokens[2].c_str());
				float z = std::stof(tokens[3].c_str());
				nor_list.push_back(DirectX::XMFLOAT3(x, y, z));
				
			} else if (type.compare("vt") == 0) {

				float x = std::stof(tokens[1].c_str());
				float y = std::stof(tokens[2].c_str());
				tex_list.push_back(DirectX::XMFLOAT2(x, y));

			} else {

				throw std::runtime_error("Unexpected type meet in reading obj file");

			}

		}

		inline UINT GetIndex(const std::string& index, const UINT maxIndex) {
			int x = atoi(index.c_str());
			if (x > 0) {
				return(x - 1);
			} else {
				//TODO never test this
				return maxIndex + x;
			}

		}

		void CreateFace(std::vector<DefaultVertex>& vertex,
						std::vector<UINT>& index, 
						const std::vector<DirectX::XMFLOAT3>& pos_list,
						const std::vector<DirectX::XMFLOAT3>& nor_list,
						const std::vector<DirectX::XMFLOAT2>& tex_list,
						const std::vector<std::string>& tokens)
		{
			assert(tokens.size() >= 4 && "Need at least three vertex to create a face");
			UINT index0 = vertex.size();
			// Create Vertices
			for (const auto& s : tokens) 
			{
				if (s.compare("f") == 0) { continue; }
				
				std::vector<std::string> v_t_n;
				ParseLine(v_t_n, s, '/');

				DefaultVertex v;
				if (v_t_n.size() == 1)	
				{
					// the line write only the vertex: v
					UINT v_num = atoi(v_t_n[0].c_str()) - 1;
					v.position = pos_list[v_num];
				}
				else 
				{
					// the line write: v// or v/t/n
					assert(v_t_n[0] != "" && "Vertex index should not be empty!");
					UINT v_num = GetIndex(v_t_n[0], pos_list.size());
					v.position = pos_list[v_num];

					if (v_t_n[1] != "") {
						UINT t_num = GetIndex(v_t_n[1], tex_list.size());
						v.texcoor = tex_list[t_num];
					}

					if (v_t_n[2] != "") {
						UINT n_num = GetIndex(v_t_n[2], nor_list.size());
						v.normal = nor_list[n_num];
					}
				}
				vertex.push_back(v);
			}

			// Create Index
			for (UINT i = index0 + 2; i < vertex.size(); ++i) {
				index.push_back(index0);
				index.push_back(i - 1);
				index.push_back(i);
			}
		}
		

	}

	void ReadMaterial(const std::string& filePath, MaterialData& mtl) 
	{
		std::ifstream infile;
		infile.open(filePath.c_str());

		std::string line;
		std::vector<std::string> tokens;

		while (std::getline(infile, line))
		{
			ParseLine(tokens, line); // Parse by space
			if (tokens.size() == 0) { continue; }
			std::string type = tokens[0];

			const char t0 = type[0];
			switch (t0)
			{
			case('K'):
			{
				assert(tokens.size() == 4 && "Too few parameters for creating Kx in mtl");
				float x = std::stof(tokens[1].c_str());
				float y = std::stof(tokens[2].c_str());
				float z = std::stof(tokens[3].c_str());
				DirectX::XMFLOAT3 value = DirectX::XMFLOAT3(x, y, z);
				if (type.compare("Ka")) {
					mtl.Ka = value;
				}
				else if (type.compare("Kd")) {
					mtl.Kd = value;
				}
				else if (type.compare("Ks")) {
					mtl.Ks = value;
				}
				else {
					throw std::runtime_error("Unexpected type meet in reading mtl file");
				}
				break;
			}
			case('T'):
				assert(tokens.size() == 2 && "Too few parameters for creating Tr in mtl file");
				mtl.d = 1 - std::stof(tokens[1].c_str());
				break;
			case('d'):
				assert(tokens.size() == 2 && "Too few parameters for creating d in mtl file");
				mtl.d = std::stof(tokens[1].c_str());
				break;
			case('N'):
				assert(tokens.size() == 2 && "Too few parameters for creating Ns in mtl file");
				mtl.Ns = std::stof(tokens[1].c_str());
				break;
			case('m'):
				assert(tokens.size() == 2 && "Too few parameters for creating texture in mtl file");
				mtl.map_Ka = tokens[1];
				break;
			default:
				break;
			}
		}
	}

	void ReadOBJ(const std::string& filePath,
				std::vector<DefaultVertex>& vertex,
				std::vector<UINT>& index) 
	{
		std::ifstream infile;
		infile.open(filePath.c_str());

		std::string line;
		std::vector<std::string> tokens;

		std::vector<DirectX::XMFLOAT3> pos_list;
		std::vector<DirectX::XMFLOAT3> nor_list;
		std::vector<DirectX::XMFLOAT2> tex_list;

		while (std::getline(infile, line)) 
		{
			ParseLine(tokens, line);
			if (tokens.size() == 0) { continue; }
			std::string type = tokens[0];
			
			const char t0 = type[0];
			switch (t0)
			{
			case('#'):
				break;
			case('v'):
				CreateVertex(pos_list, nor_list, tex_list, tokens);
				break;
			case('f'):
				CreateFace(vertex, index, pos_list, nor_list, tex_list, tokens);
				break;
			default:
				//TODO add line number
				// throw std::runtime_error("Unexpected type meet in reading obj file");
				break;
			}

		}

	}
}