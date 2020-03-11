#pragma once
#include "stdafx.h"

// Struct that store default vertex data
struct DefaultVertex 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoor;
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
				
				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				float z = atof(tokens[3].c_str());
				//float w = 0.0f;
				// contains w
				//if (tokens.size() == 5) { float w = atof(tokens[4].c_str()); }
				pos_list.push_back(DirectX::XMFLOAT3(x, y, z));
				
			} else if (type.compare("vn") == 0) {
	
				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				float z = atof(tokens[3].c_str());
				nor_list.push_back(DirectX::XMFLOAT3(x, y, z));
				
			} else if (type.compare("vt") == 0) {

				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				tex_list.push_back(DirectX::XMFLOAT2(x, y));

			} else {

				throw std::runtime_error("Unexpected type meet in reading obj file");

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
					// write only the vertex
					UINT v_num = atoi(v_t_n[0].c_str()) - 1;
					v.position = pos_list[v_num];
				}
				else 
				{
					// Write // (v/t/n)
					// TODO consider vertex without normal or tex
					UINT v_num = atoi(v_t_n[0].c_str()) - 1;
					UINT t_num = atoi(v_t_n[1].c_str()) - 1;
					UINT n_num = atoi(v_t_n[2].c_str()) - 1;
					v.position = pos_list[v_num];
					v.texcoor = tex_list[t_num];
					v.normal = nor_list[n_num];
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
				throw std::runtime_error("Unexpected type meet in reading obj file");
				break;
			}

		}

	}
}