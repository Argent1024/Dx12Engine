#pragma once
#include "stdafx.h"

// Struct that store default vertex data
struct DefaultVertex 
{
	DirectX::XMFLOAT4 position;
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

		template<class T>
		void AddToList(std::vector<T>& v, const UINT index) 
		{
			if (v.size() > index) {
				return;
			}
			v.push_back(T());
		}

		void CreateVertex(std::vector<DefaultVertex>& vertex, 
						std::vector<UINT>& count,			// contain 3 UINT, v_i, n_i, t_i
						const std::vector<std::string>& tokens)
		{
			const std::string type = tokens[0];
			
			if (type.compare("v") == 0) {
				const UINT index = count[0];
				AddToList(vertex, index);
				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				float z = atof(tokens[3].c_str());
				float w = 0.0;
				// contains w
				if (tokens.size() == 5) { float w = atof(tokens[4].c_str()); }
				vertex[index].position = DirectX::XMFLOAT4(x, y, z, w);
				count[0] += 1;
			} else if (type.compare("vn")) {
				const UINT index = count[1];
				AddToList(vertex, index);
				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				float z = atof(tokens[3].c_str());
				vertex[index].normal = DirectX::XMFLOAT3(x, y, z);
				count[1] += 1;
			} else if (type.compare("vt")) {
				const UINT index = count[2];
				AddToList(vertex, index);
				float x = atof(tokens[1].c_str());
				float y = atof(tokens[2].c_str());
				vertex[index].texcoor = DirectX::XMFLOAT2(x, y);
				count[2] += 1;
			}
			else {
				throw std::runtime_error("Unexpected type meet in reading obj file");
			}

		}

		void CreateFace(std::vector<UINT>& index, const std::vector<std::string>& tokens)
		{
			std::vector<std::string> vtn;
			for (const auto& s : tokens) {
				ParseLine(vtn, s, '/');
				UINT v_num = atoi(vtn[0].c_str());
				index.push_back(v_num);
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
		std::vector<UINT> vertexCount {0, 0, 0}; // v_i, n_i, t_i
	
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
				CreateVertex(vertex, vertexCount, tokens);
				break;
			case('f'):
				CreateFace(index, tokens);
				break;
			default:
				//TODO add line number
				throw std::runtime_error("Unexpected type meet in reading obj file");
				break;
			}

		}

	}
}