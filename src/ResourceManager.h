#pragma once

#include <map>
#include <string>

#include "DXHelper.h"
#include "Graphic/RootSignature.h"
#include "Graphic/DescriptorHeap.h"

#include "World/Mesh.h"
#include "World/Material.h"


namespace Game {
	template<class T>
	class DataManager {
	public:
		std::shared_ptr<T> Get(const std::string& key) const { 
			assert(m_map.find(key) != m_map.end());
			return m_map[key]; 
		}

		void Load(const std::string& key, std::shared_ptr<T> Assert) {
			if (m_map.find(key) == m_map.end()) {
				m_map[key] = Assert;
			}
		}

		void Remove(const std::string& key) {
			assert(m_map.find(key) != m_map.end());
			m_map.erase(m_map.find(key));
		}

	private:
		std::map<std::string, std::shared_ptr<T>> m_map;

	};


	// One ResourceManager for the entire engine
	class EngineResourceManager {
	public:
		

	private:
		// Assert Manager
		DataManager<Mesh> m_MeshManager;
		DataManager<Material> m_MaterialManager;

		// GPU Stuff
		Graphic::DescriptorHeap m_DescriptorHeap;
		DataManager<Graphic::GraphicsPSO> m_gPSO;
		DataManager<Graphic::ComputePSO> m_cPSO;
		DataManager<Graphic::RootSignature> m_RootSignatureManager;
	};
}