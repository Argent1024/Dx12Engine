#pragma once

#include "stdafx.h"

template <class Key,  class Type>
class ResourceManager
{
public:
	Type& operator[] ( const Key& key ){

	}

	void Erase(Key k) { 
		// TODO memory management here
		m_Table.erase(k); 
	}

	void Clear() { m_Table = std::map<Key, Type*>(); }

private:
	std::map<Key, Type> m_Table;

};

