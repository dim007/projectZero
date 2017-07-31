#include "stdafx.h"
#include "Component.h"
namespace GUI { //prevents having to type GUI::Component::etc

	Component::Component()
		: mIsSelected(false), mIsActive(false)
	{
	}
	Component::~Component()
	{
	}

	bool Component::isSelected() const {
		return mIsSelected;
	}
	void Component::select() {
		mIsSelected = true;
	}
	void Component::deselect() {
		mIsSelected = false;
	}
	bool Component::isActive() const {
		return mIsActive;
	}
	void Component::activate() {
		mIsActive = true;
	}
	void Component::deactivate() {
		mIsActive = false;
	}
}