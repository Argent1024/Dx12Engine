#pragma once

#include "stdafx.h"

template <class Key,  class Type>
class ResourceManager
{
public:
	Type* Find(Key k) {
		auto result = m_Table.find(k);
		if (result == m_Table.end()) { return nullptr; }
		return result->second;
	}

	void Register(Key k, Type* type) {
		assert(Find(k) == nullptr && "Key already exist");
		m_Table[Key] = type;
	}

	void Erase(Key k) { 
		// TODO memory management here
		m_Table.erase(k); 
	}

	void Clear() { m_Table = std::map<Key, Type*>(); }

private:
	std::map<Key, Type*> m_Table;

};

