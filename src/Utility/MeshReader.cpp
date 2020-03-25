#include "MeshReader.h"

void MeshReader::CreateVertex(std::vector<DirectX::XMFLOAT3>& pos_list,
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

	}
	else if (type.compare("vn") == 0) {

		float x = std::stof(tokens[1].c_str());
		float y = std::stof(tokens[2].c_str());
		float z = std::stof(tokens[3].c_str());
		nor_list.push_back(DirectX::XMFLOAT3(x, y, z));

	}
	else if (type.compare("vt") == 0) {

		float x = std::stof(tokens[1].c_str());
		float y = std::stof(tokens[2].c_str());
		tex_list.push_back(DirectX::XMFLOAT2(x, y));

	}
	else {

		throw std::runtime_error("Unexpected type meet in reading obj file");

	}
}

void MeshReader::CreateFace(
		const std::vector<DirectX::XMFLOAT3>& pos_list,
		const std::vector<DirectX::XMFLOAT3>& nor_list,
		const std::vector<DirectX::XMFLOAT2>& tex_list,
		const std::vector<std::string>& tokens)
{
	assert(tokens.size() >= 4 && "Need at least three vertex to create a face");
	UINT index0 = m_vertex.size();
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
		m_vertex.push_back(v);
	}

	// Create Index
	for (UINT i = index0 + 2; i < m_vertex.size(); ++i) {
		m_index.push_back(index0);
		m_index.push_back(i - 1);
		m_index.push_back(i);
	}
}

void MeshReader::ReadMaterial(const std::string& filePath)
{
	m_mtl = {};

	std::ifstream infile;
	infile.open(filePath.c_str());

	std::string line;
	std::vector<std::string> tokens;

	std::string mtl_name;
	while (std::getline(infile, line))
	{
		ParseLine(tokens, line); // Parse by space
		if (tokens.size() == 0) { continue; }
		std::string type = tokens[0];

		const char t0 = type[0];
		switch (t0)
		{
		case('n'):
			mtl_name = tokens[1];
			m_mtl[mtl_name] = MaterialData();
			break;
		case('K'):
		{
			assert(tokens.size() == 4 && "Too few parameters for creating Kx in mtl");
			float x = std::stof(tokens[1].c_str());
			float y = std::stof(tokens[2].c_str());
			float z = std::stof(tokens[3].c_str());
			DirectX::XMFLOAT3 value = DirectX::XMFLOAT3(x, y, z);
			if (type.compare("Ka") == 0) {
				m_mtl[mtl_name].Ka = value;
			}
			else if (type.compare("Kd") == 0) {
				m_mtl[mtl_name].Kd = value;
			}
			else if (type.compare("Ks") == 0) {
				m_mtl[mtl_name].Ks = value;
			}
			else {
				throw std::runtime_error("Unexpected type meet in reading mtl file");
			}
			break;
		}
		case('T'):
			assert(tokens.size() == 2 && "Too few parameters for creating Tr in mtl file");
			m_mtl[mtl_name].d = 1 - std::stof(tokens[1].c_str());
			break;
		case('d'):
			assert(tokens.size() == 2 && "Too few parameters for creating d in mtl file");
			m_mtl[mtl_name].d = std::stof(tokens[1].c_str());
			break;
		case('N'):
			assert(tokens.size() == 2 && "Too few parameters for creating Ns in mtl file");
			m_mtl[mtl_name].Ns = std::stof(tokens[1].c_str());
			break;
		case('m'):
			assert(tokens.size() == 2 && "Too few parameters for creating texture in mtl file");
			m_mtl[mtl_name].map_Ka = tokens[1];
			break;
		default:
			break;
		}
	}
}


void MeshReader::ReadOBJ(const std::string& filePath)
{
	m_vertex = {};
	m_index = {};

	std::ifstream infile;
	infile.open(filePath.c_str());

	std::string line;
	std::vector<std::string> tokens;

	std::vector<DirectX::XMFLOAT3> pos_list;
	std::vector<DirectX::XMFLOAT3> nor_list;
	std::vector<DirectX::XMFLOAT2> tex_list;

	std::string GroupName = "";
	std::string MTLName = "";

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
			CreateFace(pos_list, nor_list, tex_list, tokens);
			break;
		case('g'):
		{
			UINT index = m_index.size();
			// We are having a group now
			if (GroupName.compare("") != 0) {
				m_groupIndex[GroupName].end = index;
			}
			if (tokens.size() >= 2) {
				GroupName = tokens[1];
			}
			else {
				GroupName = "";
			}
			assert(m_groupIndex.find(GroupName) == m_groupIndex.end);
			m_groupIndex[GroupName] = {index, 0};
			break;
		}
		case('u'): //usemtl xxx
		{
			UINT index = m_index.size();
			// We are having a MTL now
			if (MTLName.compare("") != 0) {
				m_mtlIndex[MTLName].end = index;
			}
			if (tokens.size() >= 2) {
				MTLName = tokens[1];
			}
			else {
				MTLName = "";
			}
			assert(m_mtl.find(MTLName) != m_mtl.end() && "Using undefined material");
			m_mtlIndex[MTLName] = {index, 0};
			break;
		}
		default:
			//TODO add line number
			// throw std::runtime_error("Unexpected type meet in reading obj file");
			break;
		}

	}

	// Fill the last group and material
	UINT index = m_index.size();
	if (GroupName.compare("") != 0) {
		m_groupIndex[GroupName].end = index;
	}

	if (MTLName.compare("") != 0) {
		m_mtlIndex[MTLName].end = index;
	}
}