#pragma once
#include "FIlter.h"


class FilterManager
{
private:
	Filter& m_Filter;
	friend class Filter;
public:
	FilterManager(Filter& filter) : m_Filter(filter) {}
	void OnImGuiRender();
};