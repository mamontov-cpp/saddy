#include "../ifaceeditor.h"

#include "../../objects/abstractscreenobject.h"
#include "../../objects/screentemplate.h"

#include "../../core/editorbehaviour.h"
#include "../../core/editorbehaviourshareddata.h"

#include "selectedstate.h"

#include <algorithm>
#include <vector>

// Contains methods for changing selection in editor
void IFaceEditor::showObjectStats(AbstractScreenObject * o)
{
	if (o != NULL)
	{
		this->panel()->updateObjectStats(o);
	}
}



void IFaceEditor::trySelectObject(sad::Point2D p, bool enterSelected) 
{
	const std::vector<AbstractScreenObject*> & tbl = this->result()->fetchObjectsWithin(p);
	if (tbl.empty() == false) {
		this->behaviourSharedData()->setSelectedObject(tbl[0]);
		this->showObjectStats(tbl[0]);
		std::vector<sad::String> chain;
        for(unsigned int i = 0; i < tbl.size(); i++)
		{
			chain.push_back(tbl[i]->prop<sad::String>("uid", this->log()));
		}
		if (enterSelected) 
		{
			this->currentBehaviour()->enterState("selected");
		}
		if (chain.size() > 1) 
		{
			SelectedState * s = this->cbStateAs<SelectedState>("selected");
			s->enterNavigation(chain);
		}
	}
	this->result()->clearPickedCache();
}

