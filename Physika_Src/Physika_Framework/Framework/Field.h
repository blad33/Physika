#pragma once
#include "Physika_Core/Platform.h"
#include <typeinfo>
#include <string>
#include <cuda_runtime.h>
#include "Physika_Core/Typedef.h"

namespace Physika {
	class Base;
/*!
*	\class	Variable
*	\brief	Interface for all variables.
*/
class Field
{
public:
	Field() : m_name("default"), m_description("") {};
	Field(std::string name, std::string description) { m_name = name; m_description = description; }
	virtual ~Field() {};

	virtual size_t size() { return 0; }
	virtual const std::string getTemplateName() { return std::string(""); }
	virtual const std::string getClassName() { return std::string("Field"); }

	std::string	getObjectName() { return m_name; }
	std::string	getDescription() { return m_description; }
	virtual DeviceType getDeviceType() { return DeviceType::UNDEFINED; }

	void	setObjectName(std::string name) { m_name = name; }
	void	setDescription(std::string description) { m_description = description; }

	virtual void reset() {};

private:
	std::string m_name;
	std::string m_description;
};

}