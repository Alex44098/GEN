#pragma once

#include "Engine/EngineTypes.h"

template<class T>
class GameObject : public GECS::Entity<T> {
public:
	GameObject() {}
	~GameObject() {}
};