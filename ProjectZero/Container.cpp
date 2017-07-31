#include "stdafx.h"
#include "Container.h"
#include "SFML\Window\Event.hpp"
#include <iostream>
using namespace std;

namespace GUI {
	Container::Container()
		: mChildren(), mSelectedChild() 
	{
	}
	void Container::pack(Component::Ptr component) {
		mChildren.push_back(component);
		if (!hasSelection() && component->isSelectable()) //check if we have selected a child, if not and if comp is selectable then select it
			select(mChildren.size() - 1);//the recently pushed back one
	}

	bool Container::isSelectable() const {
		return false;
	}

	bool Container::hasSelection() const {
		return mSelectedChild >= 0;
	}

	void Container::handleEvent(const sf::Event& event) {
		if (hasSelection() && mChildren[mSelectedChild]->isActive()) { //if valid index && isActive go to the component for event
			mChildren[mSelectedChild]->handleEvent(event);
		}
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
				selectPrevious();
			}
			else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
				selectNext();
			}
			else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
				if (hasSelection())
					mChildren[mSelectedChild]->activate();
			}
		}
	}
	void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform(); //from transformable extended file
		for (const Component::Ptr& child : mChildren)
			target.draw(*child, states);
	}
	void Container::select(std::size_t index) {
		if (mChildren[index]->isSelectable()) {
			if (hasSelection())
				mChildren[mSelectedChild]->deselect(); //unselect old value if it has one in mSelectedChild
			mChildren[index]->select(); //select new one
			mSelectedChild = index;
		}
	}
	void Container::selectNext() {
		if (!hasSelection()) //cant move to next if currently has none selected
			return;
		int next = mSelectedChild;
		do
			next = (next + 1) % mChildren.size(); //loops through in a circle with MOD
		while (!mChildren[next]->isSelectable()); //stop when you find a selectable option
		//then select component
		select(next);
	}
	void Container::selectPrevious() {
		if (!hasSelection())
			return;
		int prev = mSelectedChild;
		do
			prev = (prev + mChildren.size() - 1) % mChildren.size(); //logic to loop backwards
		while (!mChildren[prev]->isSelectable()); //stop when found selectable option
		//select
		select(prev);
	}

}